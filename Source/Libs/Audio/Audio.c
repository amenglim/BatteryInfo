// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//                         COPYRIGHT NOTICE
//                   "Copyright 2021 Nova Biomedical Corporation"
//             This program is the property of Nova Biomedical Corporation
//                 200 Prospect Street, Waltham, MA 02454-9141
//             Any unauthorized use or duplication is prohibited
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
/// Title            -  Audio Device interface
/// Source Filename  -  Audio.c
/// Originator       -  CWong
/// Description      -  An interface to control the audio of the device
///
/// Dependencies     -  Speaker
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include <pthread.h>
#include <stdint.h>
#include <stdatomic.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "debug.hpp"
#include "Audio.h"
#include "Speaker.h"
#include "Music.h"


#define MSG_Q_LENGTH 10
#define UNUSED(x) (void)(x)

enum ComponentState
{
    Ended,
    Inited,
    Running,
    Ending,
};

enum ProcessCode
{
    CODE_OK,
    CODE_KILL,
};

enum ErrorCodes
{
    ERROR_NONE,
    ERROR_TASK_RUNNING,
    ERROR_TASK_STOPPED,
    ERROR_TASK_CREATE,
    ERROR_NULL_MSG_QUEUE,
    ERROR_GET_HEADER,
    ERROR_UNKNOWN_MESSAGE,
    ERROR_SET_VOLUME,
    ERROR_MUTEX,
    ERROR_MUTEX_CREATE,
    ERROR_MUTEX_DESTROY,
    ERROR_COND_CREATE,
    ERROR_COND_DESTROY,
    ERROR_MSGQ_FULL,
    ERROR_HARDWARE_INIT,
    ERROR_HARDWARE_DEINIT,
};

enum MessageID
{
    MSG_ID_END,
    MSG_ID_PLAY_TRACK,
};

struct Message
{
    enum MessageID MsgID;
    union Data
    {
        AudioTrackID TrackID;
    } Data;
};

static atomic_int MyState = Ended;
static atomic_int PlayerState = Ended;
static pthread_t AudioThread;
static pthread_mutex_t AudioMutex = PTHREAD_MUTEX_INITIALIZER;

static atomic_int NextTrackID = MaxTracks;
static int GetTrack(AudioTrackID TrackID, const uint16_t ** Melody, const uint16_t ** Rhythm, size_t * Len);

static pthread_mutex_t MsgMutex;
static pthread_cond_t MsgCond;
static struct Message MsgQ[MSG_Q_LENGTH] = { 0 };
static int MsgQLen = 0;
static int TopMsgIndex = 0;

static void * TaskLoop(void *);
static void * PlayerTaskLoop(void *);
static int ProcessMessage(struct Message * Message);
static int PlayTrackID(AudioTrackID TrackID);

static int InitMessaging(void);
static int GetMessage(struct Message * Message);
static int SendMessage(struct Message * Message);

void RunAudioPlayer(void)
{
	int error = ERROR_NONE;

	if ((error = Audio_init()) != ERROR_NONE)
	{
		DBGPRT(DBG_ERR, "RunAudioPlayer: Audio init error - %d - %s\n", error, strerror(errno));
	}

	if ((error = Audio_start()) != ERROR_NONE)
	{
		DBGPRT(DBG_ERR, "RunAudioPlayer: Audio start error - %d - %s\n", error, strerror(errno));
	}

	if ((error = Audio_SetVolume(Audio_GetMaxVolume())) != ERROR_NONE)
	{
		DBGPRT(DBG_ERR, "RunAudioPlayer: Audio set volume error - %d - %s\n", error, strerror(errno));
	}
}

void CloseAudioPlayer(void)
{
	int error = ERROR_NONE;

	if ((error = Audio_end()) != ERROR_NONE)
	{
		DBGPRT(DBG_ERR, "RunAudioPlayer: Audio end error - %d - %s\n", error, strerror(errno));
	}
}

int Audio_init(void)
{
    int Error = ERROR_NONE;
    if (pthread_mutex_lock(&AudioMutex))
    {
        return ERROR_MUTEX;
    }
    if (MyState != Ended)
    {
        Error = ERROR_TASK_RUNNING;
        goto END;
    }

    if (InitMessaging())
    {
        Error = ERROR_NULL_MSG_QUEUE;
        goto END;
    }

    if (Speaker_Init())
    {
        Error = ERROR_HARDWARE_INIT;
        goto END;
    }

    MyState = Inited;

END:
    pthread_mutex_unlock(&AudioMutex);
    return Error;
}

int Audio_start(void)
{
    int Error = ERROR_NONE;
    if (pthread_mutex_lock(&AudioMutex))
    {
        return ERROR_MUTEX;
    }
    if (MyState != Inited)
    {
        Error = ERROR_TASK_RUNNING;
    goto END;
    }

    if (pthread_create(&AudioThread, NULL, TaskLoop, NULL))
    {
        Error = ERROR_TASK_CREATE;
        goto END;
    }
    else
    {
        MyState = Running;
    }
END:
    pthread_mutex_unlock(&AudioMutex);
    return Error;
}

int Audio_end(void)
{
    int Error = ERROR_NONE;
    if (pthread_mutex_lock(&AudioMutex))
    {
        return ERROR_MUTEX;
    }
    if (MyState == Ended)
    {
        // Maybe add error
        goto END;
    }
    if (MyState == Running)
    {
        MyState = Ending;
        struct Message Msg =
        {
            .MsgID = MSG_ID_END,
        };
        SendMessage(&Msg);
        pthread_join(AudioThread, NULL); // Need error check
    }
    if ((MyState == Ending) || (MyState == Inited))
    {
        int SpeakerErrorCode = Speaker_DeInit();
	// Don't care if speaker is in ended state, so ignore error
	if ((SpeakerErrorCode != SPEAKER_ERROR_NONE) && (SpeakerErrorCode != SPEAKER_ERROR_STATE))
	{
            Error = ERROR_HARDWARE_DEINIT;
	}

        if (pthread_mutex_destroy(&MsgMutex))
        {
            Error = ERROR_MUTEX_DESTROY;
        }
        if (pthread_cond_destroy(&MsgCond))
        {
            Error = ERROR_COND_DESTROY;
        }

        MsgQLen = 0;

        if (Error == ERROR_NONE)
        {
            MyState = Ended;
        }
    }
    else
    {
        Error = ERROR_TASK_RUNNING;
    }
END:
    pthread_mutex_unlock(&AudioMutex);
    return Error;
}

int Audio_PlayTrack(AudioTrackID TrackID)
{
    if (MyState == Running)
    {
        struct Message Msg =
        {
            .MsgID = MSG_ID_PLAY_TRACK,
            .Data =
            {
                .TrackID = TrackID,
            },
        };
        return SendMessage(&Msg);
    }

    return ERROR_TASK_STOPPED;
}

int Audio_SetVolume(uint32_t Level)
{
    int result = ERROR_NONE;

    if (Speaker_SetVolume(Level))
    {
	    result = ERROR_SET_VOLUME;
    }

    return result;
}

uint32_t Audio_GetMaxVolume(void)
{
    return Speaker_GetMaxVolume();
}

static void * TaskLoop(void * args)
{
    UNUSED(args);
    pthread_mutex_lock(&MsgMutex);
    pthread_t AudioPlayerThread;
    PlayerState = Running;
    pthread_create(&AudioPlayerThread, NULL, PlayerTaskLoop, NULL); // Add error
    while(MyState == Running)
    {
        struct Message Msg;
        while (GetMessage(&Msg) != 0)
        {
            pthread_cond_wait(&MsgCond, &MsgMutex);
        }
        pthread_mutex_unlock(&MsgMutex);

        if (ProcessMessage(&Msg) == CODE_KILL)
        {
            pthread_mutex_lock(&MsgMutex);
            goto END;
        }
        pthread_mutex_lock(&MsgMutex);
    }

END:
    MyState = Ending;
    pthread_mutex_unlock(&MsgMutex);
    PlayerState = Ending;
    pthread_join(AudioPlayerThread, NULL); // Need error check
    PlayerState = Ended;
    return NULL;
}

static void * PlayerTaskLoop(void * arg)
{
    UNUSED(arg);
    int TickCount = 0;
    unsigned int NoteIndex = 0;
    AudioTrackID CurrTrackID = MaxTracks;
    const uint16_t * Melody = NULL;
    const uint16_t * Rhythm = NULL;
    size_t Len = 0;

    while (PlayerState == Running)
    {
        usleep(1000);
        AudioTrackID TempNextTrackID = atomic_exchange(&NextTrackID, MaxTracks);
        if (TempNextTrackID != MaxTracks)
        {
            NoteIndex = 0;
            TickCount = 0;
            Melody = NULL;
            Rhythm = NULL;
            CurrTrackID = TempNextTrackID;
        }
        if (CurrTrackID >= MaxTracks)
        {
            Speaker_PlayPitch(0);
            continue;
        }
        if ((NoteIndex == 0) && ((Melody == NULL) || (Rhythm == NULL)))
        {
            if (GetTrack(CurrTrackID, &Melody, &Rhythm, &Len) != 0)
            {
                Melody = NULL;
                Rhythm = NULL;
                CurrTrackID = MaxTracks;
                Speaker_PlayPitch(0);
                continue;
            }
        }
        if ((Melody == NULL) || (Rhythm == NULL) || (NoteIndex >= Len))
        {
            CurrTrackID = MaxTracks;
            Speaker_PlayPitch(0);
            continue;
        }
        if (TickCount == 0)
        {
            Speaker_PlayPitch(Melody[NoteIndex]);
        }
        if (TickCount >= Rhythm[NoteIndex])
        {
            TickCount = 0;
            NoteIndex++;
            Speaker_PlayPitch(0);
            usleep(NOTE_SEPARATION);
        }
        else
        {
            TickCount++;
        }

        if (NoteIndex >= Len)
        {
            NoteIndex = 0;
            Melody = NULL;
            Rhythm = NULL;
            CurrTrackID = MaxTracks;
            Len = 0;
        }
    }
    NextTrackID = MaxTracks;
    Speaker_PlayPitch(0);

    return NULL;
}

static int GetTrack(AudioTrackID TrackID, const uint16_t ** Melody, const uint16_t ** Rhythm, size_t * Len)
{
    int Error = 0;
    switch (TrackID)
    {
        case Scale:
        {
        *Melody = Scale_Melody;
        *Rhythm = Scale_Rhythm;
        *Len = sizeof(Scale_Melody) / sizeof(*(Scale_Melody));
            break;
        }
        case Alarm1:
        {
        *Melody = Alarm1_Melody;
        *Rhythm = Alarm1_Rhythm;
        *Len = sizeof(Alarm1_Melody) / sizeof(*(Alarm1_Melody));
            break;
        }
        case Alarm2:
        {
        *Melody = Alarm2_Melody;
        *Rhythm = Alarm2_Rhythm;
        *Len = sizeof(Alarm2_Melody) / sizeof(*(Alarm2_Melody));
            break;
        }
        case Alert1:
        {
        *Melody = Alert1_Melody;
        *Rhythm = Alert1_Rhythm;
        *Len = sizeof(Alert1_Melody) / sizeof(*(Alert1_Melody));
            break;
        }
        case Alert2:
        {
        *Melody = Alert2_Melody;
        *Rhythm = Alert2_Rhythm;
        *Len = sizeof(Alert2_Melody) / sizeof(*(Alert2_Melody));
            break;
        }
        case BadInput:
        {
        *Melody = BadInput_Melody;
        *Rhythm = BadInput_Rhythm;
        *Len = sizeof(BadInput_Melody) / sizeof(*(BadInput_Melody));
            break;
        }
        case BootTune:
        {
        *Melody = BootTune_Melody;
        *Rhythm = BootTune_Rhythm;
        *Len = sizeof(BootTune_Melody) / sizeof(*(BootTune_Melody));
           break;
        }
        case Docked:
        {
        *Melody = Docked_Melody;
        *Rhythm = Docked_Rhythm;
        *Len = sizeof(Docked_Melody) / sizeof(*(Docked_Melody));
            break;
        }
        case KeyPress:
        {
        *Melody = KeyPress_Melody;
        *Rhythm = KeyPress_Rhythm;
        *Len = sizeof(KeyPress_Melody) / sizeof(*(KeyPress_Melody));
            break;
        }
        case Long:
        {
        *Melody = Long_Melody;
        *Rhythm = Long_Rhythm;
        *Len = sizeof(Long_Melody) / sizeof(*(Long_Melody));
            break;
        }
        case Quick:
        {
        *Melody = Quick_Melody;
        *Rhythm = Quick_Rhythm;
        *Len = sizeof(Quick_Melody) / sizeof(*(Quick_Melody));
            break;
        }
        case QuickThree:
        {
        *Melody = QuickThree_Melody;
        *Rhythm = QuickThree_Rhythm;
        *Len = sizeof(QuickThree_Melody) / sizeof(*(QuickThree_Melody));
            break;
        }
        case Scan:
        {
        *Melody = Scan_Melody;
        *Rhythm = Scan_Rhythm;
        *Len = sizeof(Scan_Melody) / sizeof(*(Scan_Melody));
            break;
        }
        case Nova:
        {
        *Melody = Nova_Melody;
        *Rhythm = Nova_Rhythm;
        *Len = sizeof(Nova_Melody) / sizeof(*(Nova_Melody));
            break;
        }
        default:
        {
        Error = -1;
            break;
        }
    }
    return Error;
}

static int ProcessMessage(struct Message * Message)
{
    switch(Message->MsgID)
    {
    case MSG_ID_PLAY_TRACK:
        PlayTrackID(Message->Data.TrackID);
        break;
    case MSG_ID_END:
        return CODE_KILL;
    default:
        break;
    }

    return CODE_OK;
}

static int PlayTrackID(AudioTrackID TrackID)
{
    NextTrackID = TrackID;
    return 0;
}

static int InitMessaging(void)
{
    if (pthread_mutex_init(&MsgMutex, NULL))
    {
        return ERROR_MUTEX_CREATE;
    }
    if (pthread_cond_init(&MsgCond, NULL))
    {
        return ERROR_COND_CREATE;
    }

    MsgQLen = 0;
    TopMsgIndex = 0;

    return ERROR_NONE;
}

static int GetMessage(struct Message * Message)
{
    if (MsgQLen < 1)
    {
        return -1;
    }
    memcpy(Message, &(MsgQ[TopMsgIndex]), sizeof(*Message));

    TopMsgIndex = (TopMsgIndex + 1) % MSG_Q_LENGTH;
    if (MsgQLen > 0)
    {
        MsgQLen--;
    }

    return 0;
}

static int SendMessage(struct Message * Message)
{
    int Error = ERROR_NONE;
    if (pthread_mutex_lock(&MsgMutex))
    {
        return ERROR_MUTEX;
    }
    if (MsgQLen < MSG_Q_LENGTH)
    {
        int WriteIndex = (TopMsgIndex + MsgQLen) % MSG_Q_LENGTH;
        memcpy(&(MsgQ[WriteIndex]), Message, sizeof(MsgQ[0]));
        MsgQLen++;
        pthread_cond_signal(&MsgCond);
    }
    else
    {
        Error = ERROR_MSGQ_FULL;
    }
    pthread_mutex_unlock(&MsgMutex);
    return Error;
}
