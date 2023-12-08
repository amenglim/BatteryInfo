/*******************************************************************************
*                         COPYRIGHT NOTICE
*                   "Copyright 2023 Nova Biomedical Corporation"
*             This program is the property of Nova Biomedical Corporation
*                 200 Prospect Street, Waltham, MA 02454-9141
*             Any unauthorized use or duplication is prohibited
********************************************************************************
*
*  Title            -
*  Source Filename  -
*  Author           -
*  Description      -
*
*******************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <map>
#include <pthread.h>

#include "debug.hpp"

#define MAJOR_VER       "1"
#define MINOR_VER       "0"
#define BUILD_VER       "0"
#define PATCH_VER       "0"

#define MAX_T_IDS       (10)
#define BRDR_WIDTH      (10)
#define BTN_WIDTH       (220)
#define BTN_HEIGHT      (100)
#define STRT_X          ((CANVAS_WIDTH / 2) - (BTN_WIDTH / 2))
#define STRT_Y          (CANVAS_HEIGHT - BTN_HEIGHT - BRDR_WIDTH)
#define GPIOD_API		__attribute__((visibility("default")))

typedef void *(*func_ptr)(void*);





