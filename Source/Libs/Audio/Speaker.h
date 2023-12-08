// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//                         COPYRIGHT NOTICE
//                   "Copyright 2021 Nova Biomedical Corporation"
//             This program is the property of Nova Biomedical Corporation
//                 200 Prospect Street, Waltham, MA 02454-9141
//             Any unauthorized use or duplication is prohibited
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
/// Title            -  Speaker interface
/// Source Filename  -  Speaker.hpp
/// Originator       -  Chris Wong
/// Description      -
///
/// Dependencies     -  <none>
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum Speaker_ErrorCodes
{
	SPEAKER_ERROR_NONE,
	SPEAKER_ERROR_SYSFS,
	SPEAKER_ERROR_OUT_OF_BOUNDS,
	SPEAKER_ERROR_STATE,
	SPEAKER_ERROR_MUTEX,
};

int Speaker_Init(void);
int Speaker_PlayPitch(uint16_t Frequency);
int Speaker_SetVolume(uint32_t Level);
uint32_t Speaker_GetMaxVolume(void);
int Speaker_DeInit(void);

#ifdef __cplusplus
}
#endif
