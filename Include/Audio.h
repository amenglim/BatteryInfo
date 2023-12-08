// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//                         COPYRIGHT NOTICE
//                   "Copyright 2021 Nova Biomedical Corporation"
//             This program is the property of Nova Biomedical Corporation
//                 200 Prospect Street, Waltham, MA 02454-9141
//             Any unauthorized use or duplication is prohibited
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
/// Title            -  Audio device access
/// Source Filename  -  Audio.h
/// Originator       -  CWong
/// Description      -
///
/// Dependencies     -  <none>
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "AudioTrackID.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void RunAudioPlayer(void);
void CloseAudioPlayer(void);

int Audio_init(void);

int Audio_start(void);

int Audio_end(void);

int Audio_PlayTrack(AudioTrackID TrackID);
int Audio_SetVolume(uint32_t Level);
uint32_t Audio_GetMaxVolume(void);

#ifdef __cplusplus
}
#endif
