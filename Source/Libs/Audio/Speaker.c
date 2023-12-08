// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//                         COPYRIGHT NOTICE
//                   "Copyright 2023 Nova Biomedical Corporation"
//             This program is the property of Nova Biomedical Corporation
//                 200 Prospect Street, Waltham, MA 02454-9141
//             Any unauthorized use or duplication is prohibited
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
/// Title            -  Speaker supported functionality
/// Source Filename  -  Speaker.c
/// Originator       -  CWong
/// Description      -  An interface to control the speaker hardware
///
/// Dependencies     -  libgpiod
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#include "Speaker.h"

#include <gpiod.h>

#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define SPEAKER_MAX_VOLUME_LEVEL 3

#ifndef SPEAKER_PWM_DEV_DIR
#define SPEAKER_PWM_DEV_DIR "/sys/devices/platform/soc/40002000.timer/40002000.timer:pwm/pwm/"
#endif
#define SPEAKER_PWM_DIR_LEN (15 + 10)
#define PWM_DIR "/sys/class/pwm/"
#define CHANNEL_ID 0
#define GPIO_CHIPNUM_MAX 9

#define GPIO_OFFSET_SPKR_EN1 8
#define GPIO_OFFSET_SPKR_EN2 9

#define SPKR_EN1_BIT 0x02
#define SPKR_EN2_BIT 0x01

enum Speaker_GPIO_ErrorCodes
{
	SPEAKER_GPIO_ERROR_NONE,
	SPEAKER_GPIO_ERROR_OPEN_CHIP,
	SPEAKER_GPIO_ERROR_GET_LINE,
};

enum Speaker_State
{
	SPEAKER_INITED,
	SPEAKER_ENDED,
};

static enum Speaker_State State = SPEAKER_ENDED;
static pthread_mutex_t SpeakerMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t VolumeMutex = PTHREAD_MUTEX_INITIALIZER;

static char SpeakerDir[SPEAKER_PWM_DIR_LEN] = "";

static struct gpiod_line * speaker_en1_line = NULL;
static struct gpiod_line * speaker_en2_line = NULL;

static const char * chip_name = "gpiochip7";
static struct gpiod_chip * chip = NULL;

static int Export(void);
static int OpenVolumePins(void);
static int CloseVolumePins(void);
static int PWMFileWriteInt(char * SubDir, int Write);
static int PWMWrite(char * SubDir, int Write);

int Speaker_Init(void)
{
	int error = SPEAKER_ERROR_NONE;
	if (pthread_mutex_lock(&SpeakerMutex))
	{
		return SPEAKER_ERROR_MUTEX;
	}

	if (State != SPEAKER_ENDED)
	{
		error = SPEAKER_ERROR_STATE;
		goto END;
	}

	DIR * d;
	struct dirent * dir;
	d = opendir(SPEAKER_PWM_DEV_DIR);
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if ((dir->d_name)[0] != '.')
			{
				snprintf(SpeakerDir, sizeof(SpeakerDir), "%s%s", PWM_DIR, dir->d_name);
			}
		}
	}
	else
	{
		error = SPEAKER_ERROR_SYSFS;
		goto END;
	}
	closedir(d);
	error = Export();
	if (error != SPEAKER_ERROR_NONE)
	{
		goto END;
	}

	error = OpenVolumePins();
	if (error != SPEAKER_ERROR_NONE)
	{
		goto END;
	}

	State = SPEAKER_INITED;
END:
    pthread_mutex_unlock(&SpeakerMutex);
    return error;
}

int Speaker_PlayPitch(uint16_t Frequency)
{
	int error = SPEAKER_ERROR_NONE;
	if (pthread_mutex_lock(&SpeakerMutex))
	{
		return SPEAKER_ERROR_MUTEX;
	}

	if (State != SPEAKER_INITED)
	{
		error = SPEAKER_ERROR_STATE;
		goto END;
	}
	static uint16_t CurrentFreq = 0;
	unsigned long long DutyCyclens = 0;
	unsigned long long Periodns = 0;
	if (Frequency == CurrentFreq)
	{
		goto END;
	}
    if (Frequency == 0)
    {
        DutyCyclens = 0;
    }
    else
    {
    	Periodns = ((1.0 / (double) Frequency)) / 1E-9;
    	DutyCyclens = Periodns / 2;
    	PWMWrite("period", Periodns);
    }

    PWMWrite("duty_cycle", DutyCyclens);

    if (Frequency == 0)
    {
	    PWMWrite("enable", 0);
    }
    else
    {
	    PWMWrite("enable", 1);
    }

    CurrentFreq = Frequency;
END:
	pthread_mutex_unlock(&SpeakerMutex);

    return error;
}

int Speaker_SetVolume(uint32_t Level)
{
	int set_val = 0;
	int error = SPEAKER_ERROR_NONE;

	if (pthread_mutex_lock(&VolumeMutex))
	{
		return SPEAKER_ERROR_MUTEX;
	}

	if ((speaker_en1_line) == NULL || (speaker_en2_line == NULL))
	{
		error = SPEAKER_ERROR_SYSFS;
		goto END;
	}
	if (Level > SPEAKER_MAX_VOLUME_LEVEL)
	{
		error = SPEAKER_ERROR_OUT_OF_BOUNDS;
		goto END;
	}
	if ((Level & SPKR_EN2_BIT) == SPKR_EN2_BIT)
	{
		set_val = 1;
	}
	error |= gpiod_line_set_value(speaker_en2_line, set_val);

	if ((Level & SPKR_EN1_BIT) == SPKR_EN1_BIT)
	{
		set_val = 1;
	}
	else
	{
		set_val = 0;
	}
	error |= gpiod_line_set_value(speaker_en1_line, set_val);
	if (error != 0)
	{
		error = SPEAKER_ERROR_SYSFS;
		goto END;
	}
END:
	pthread_mutex_unlock(&VolumeMutex);
	return error;
}

uint32_t Speaker_GetMaxVolume(void)
{
    return SPEAKER_MAX_VOLUME_LEVEL;
}

int Speaker_DeInit(void)
{
	int error = SPEAKER_ERROR_NONE;
	if (pthread_mutex_lock(&SpeakerMutex))
	{
		return SPEAKER_ERROR_MUTEX;
	}

	if (State != SPEAKER_INITED)
	{
		error = SPEAKER_ERROR_STATE;
		goto END;
	}

	PWMWrite("duty_cycle", 0);
	PWMWrite("enable", 0);

	CloseVolumePins();

	// Could unexport PWM channel, but this app ignores device busy errors, and assumes that it has exclusive access to the PWM channel

	State = SPEAKER_ENDED;

END:
	pthread_mutex_unlock(&SpeakerMutex);
	return error;
}

static int Export(void)
{
	int error;

	error = PWMFileWriteInt("export", CHANNEL_ID);
	if (error != SPEAKER_ERROR_NONE)
	{
		if (errno == EBUSY)
		{
			// Channel was already exported (hopefully).  Assume that this app has exclusive access to the PWM channel
			error = SPEAKER_ERROR_NONE;
		}
	}

	return error;
}

static int PWMFileWriteInt(char * SubDir, int Write)
{
	int error;
	FILE * fptr;
	size_t subdir_maxlen = 255;
	unsigned int subdir_strlen = strnlen(SubDir, subdir_maxlen);
	int len = strnlen(SpeakerDir, sizeof(SpeakerDir)) + subdir_strlen + 2;
	char * subdir_file = malloc(len);
	snprintf(subdir_file, len, "%s/%s", SpeakerDir, SubDir);
	fptr = fopen(subdir_file, "w");
	if (fptr == NULL)
	{
		free(subdir_file);
		return SPEAKER_ERROR_SYSFS;
	}
	setvbuf(fptr, NULL, _IONBF, 0);
	error = fprintf(fptr, "%d", Write);
	if (error < 0)
	{
		error = SPEAKER_ERROR_SYSFS;
	}
	else
	{
		error = SPEAKER_ERROR_NONE;
	}
	fclose(fptr);
	free(subdir_file);
	return error;
}

static int PWMWrite(char * SubDir, int Write)
{
	int error;

	size_t subdir_maxlen = 64;
	char full_subdir[subdir_maxlen];
	snprintf(full_subdir, sizeof(full_subdir), "pwm%d/%s", CHANNEL_ID, SubDir);

	error = PWMFileWriteInt(full_subdir, Write);

	return error;
}

static int OpenVolumePins(void)
{
	// Eventually for loop through all chips searching for SPKR pins by name
	int error = SPEAKER_ERROR_NONE;
	const struct gpiod_line_request_config config = {
		.consumer = "SS2",
		.request_type = GPIOD_LINE_REQUEST_DIRECTION_OUTPUT,
		.flags = GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLE
	};

	if (pthread_mutex_lock(&VolumeMutex))
	{
		return SPEAKER_ERROR_MUTEX;
	}

	if (chip == NULL)
	{
		chip = gpiod_chip_open_by_name(chip_name);
	}
	if (chip == NULL)
	{
		error = SPEAKER_GPIO_ERROR_OPEN_CHIP;
		goto END;
	}

	speaker_en1_line = gpiod_chip_get_line(chip, GPIO_OFFSET_SPKR_EN1);
	if (speaker_en1_line == NULL)
	{
		error = SPEAKER_GPIO_ERROR_GET_LINE;
		goto END;
	}
	if (gpiod_line_request(speaker_en1_line, &config, 0) != 0)
	{
		error = SPEAKER_GPIO_ERROR_GET_LINE;
		goto END;
	}

	speaker_en2_line = gpiod_chip_get_line(chip, GPIO_OFFSET_SPKR_EN2);
	if (speaker_en2_line == NULL)
	{
		error = SPEAKER_GPIO_ERROR_GET_LINE;
		goto END;
	}
	if (gpiod_line_request(speaker_en2_line, &config, 0) != 0)
	{
		error = SPEAKER_GPIO_ERROR_GET_LINE;
		goto END;
	}

END:
	pthread_mutex_unlock(&VolumeMutex);
	return error;
}

static int CloseVolumePins(void)
{
	int error = SPEAKER_ERROR_NONE;
	if (pthread_mutex_lock(&VolumeMutex))
	{
		return SPEAKER_ERROR_MUTEX;
	}

	if (speaker_en2_line != NULL)
	{
		error |= gpiod_line_set_value(speaker_en2_line, 0);

		gpiod_line_release(speaker_en2_line);
		speaker_en2_line = NULL;
	}

	if (speaker_en1_line != NULL)
	{
		error |= gpiod_line_set_value(speaker_en1_line, 0);

		gpiod_line_release(speaker_en1_line);
		speaker_en1_line = NULL;
	}

	if (chip != NULL)
	{
		gpiod_chip_close(chip);
		chip = NULL;
	}

	pthread_mutex_unlock(&VolumeMutex);
	return error;
}
