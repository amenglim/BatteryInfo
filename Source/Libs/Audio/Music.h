// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//                         COPYRIGHT NOTICE
//                   "Copyright 2019 SANVITA MEDICAL"
//             This program is the property of SANVITA MEDICAL
//                 165 Lexington Road, Billerica, MA 01821
//             Any unauthorized use or duplication is prohibited
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
//  Header Filename  -  Music.h
//  Originator       -  C. Wong
//  Description      -  Supplements the audio hardware controller
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#pragma once

//  Include Files
#include <stdint.h>

//  Type Definitions
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#define REST_NOTE   0

#define NOTE_SEPARATION 10

#define BEAT_TIME             300

#define WHOLE_AND_HALF_NOTE     ((BEAT_TIME * 6) - NOTE_SEPARATION)
#define WHOLE_NOTE              ((BEAT_TIME * 4) - NOTE_SEPARATION)
#define HALF_NOTE               ((BEAT_TIME * 2) - NOTE_SEPARATION)
#define QUARTER_NOTE            (BEAT_TIME - NOTE_SEPARATION)
#define QUARTER_AND_HALF_NOTE   ((BEAT_TIME + (BEAT_TIME / 2)) - NOTE_SEPARATION)
#define THREE_QUARTER_NOTE      ((BEAT_TIME - 75) - NOTE_SEPARATION)
#define EIGHTH_NOTE             ((BEAT_TIME / 2) - NOTE_SEPARATION)
#define TRIPLET_NOTE            ((BEAT_TIME / 3) - NOTE_SEPARATION)
#define SIXTEENTH_NOTE          ((BEAT_TIME / 4) - NOTE_SEPARATION)

#define WHOLE_AND_HALF_REST     (WHOLE_AND_HALF_NOTE + NOTE_SEPARATION)
#define WHOLE_REST              (WHOLE_NOTE + NOTE_SEPARATION)
#define HALF_REST               (HALF_NOTE + NOTE_SEPARATION)
#define QUARTER_REST            (QUARTER_NOTE + NOTE_SEPARATION)
#define QUARTER_AND_HALF_REST   (QUARTER_AND_HALF_NOTE + NOTE_SEPARATION)
#define THREE_QUARTER_REST      (THREE_QUARTER_NOTE + NOTE_SEPARATION)
#define EIGHTH_REST             (EIGHTH_NOTE + NOTE_SEPARATION)
#define TRIPLET_REST            (TRIPLET_NOTE + NOTE_SEPARATION)
#define SIXTEENTH_REST          (SIXTEENTH_NOTE + NOTE_SEPARATION)

#ifdef NOTE_EQUIVALENT
#define NOTE_980_HZ NOTE_B5
#define NOTE_1120_HZ NOTE_CS6
#define NOTE_1800_HZ NOTE_A6
#define NOTE_2000_HZ NOTE_B6
#define NOTE_2500_HZ NOTE_DS7
#define NOTE_3200_HZ NOTE_G7
#else
#define NOTE_980_HZ 980
#define NOTE_1120_HZ 1120
#define NOTE_1800_HZ 1800
#define NOTE_2000_HZ 2500
#define NOTE_2500_HZ 2000
#define NOTE_3200_HZ 3200
#endif

#if 0
uint16_t Alarm1_Melody[] =
{
    NOTE_1120_HZ,
    NOTE_1800_HZ,
    REST_NOTE,
    NOTE_1120_HZ,
    NOTE_1800_HZ,
    REST_NOTE,
    NOTE_1120_HZ,
    NOTE_1800_HZ,
};

uint16_t Alarm1_Rhythm[] =
{
    300,
    300,
    150,
    300,
    300,
    150,
    300,
    300,
};
#endif

uint16_t Alarm1_Melody[] =
{
    NOTE_G4,
    REST_NOTE,
    NOTE_E4,
    REST_NOTE,
    NOTE_C4,
    REST_NOTE,
};

uint16_t Alarm1_Rhythm[] =
{
    SIXTEENTH_NOTE,    
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
};


uint16_t Alarm2_Melody[] =
{
    NOTE_2500_HZ,
    NOTE_3200_HZ,
    REST_NOTE,
    NOTE_2500_HZ,
    NOTE_3200_HZ,
    REST_NOTE,
    NOTE_2500_HZ,
    NOTE_3200_HZ,
};

uint16_t Alarm2_Rhythm[] =
{
    300,
    300,
    100,
    300,
    300,
    100,
    300,
    300,
};

#if 0
uint16_t Alert1_Melody[] =
{
    NOTE_980_HZ,
    NOTE_1120_HZ,
    NOTE_980_HZ,
};

uint16_t Alert1_Rhythm[] =
{
    100,
    100,
    100,
};
#endif

uint16_t Alert1_Melody[] =
{
    NOTE_C3,
    REST_NOTE,
    NOTE_E3,
    REST_NOTE,
};

uint16_t Alert1_Rhythm[] =
{
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
};


#if 0
uint16_t Alert2_Melody[] =
{
    NOTE_980_HZ,
    NOTE_1120_HZ,
    NOTE_980_HZ,
};

uint16_t Alert2_Rhythm[] =
{
    300,
    300,
    300,
};
#endif

uint16_t Alert2_Melody[] =
{
    NOTE_C4,
    REST_NOTE,
    NOTE_E4,
    REST_NOTE,
    NOTE_G4,
    REST_NOTE,
};

uint16_t Alert2_Rhythm[] =
{
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
};


uint16_t BadInput_Melody[] =
{
    NOTE_C3,
};

uint16_t BadInput_Rhythm[] =
{
    500,
};

uint16_t OdeToJoy_Melody[] =
{
#ifdef REAL_ODE_TO_JOY
    NOTE_FS5,
    NOTE_FS5,
    NOTE_G5,
    NOTE_A5,
    NOTE_A5,
    NOTE_G5,
    NOTE_FS5,
    NOTE_E5,
    NOTE_D5,
    NOTE_E5,
    NOTE_FS5,
    NOTE_FS5,
    NOTE_E5,
    NOTE_E5,
#else
    NOTE_B5,
    REST_NOTE,
    NOTE_B5,
    REST_NOTE,
    NOTE_C5,
    REST_NOTE,
    NOTE_D5,
    REST_NOTE,
    NOTE_D5,
    REST_NOTE,
    NOTE_C5,
    REST_NOTE,
    NOTE_B5,
    REST_NOTE,
    NOTE_A5,
    REST_NOTE,

    NOTE_G5,
    REST_NOTE,
    NOTE_G5,
    REST_NOTE,
    NOTE_A5,
    REST_NOTE,
    NOTE_B5,
    REST_NOTE,
    NOTE_B5,
    REST_NOTE,
    NOTE_A5,
    REST_NOTE,
    NOTE_A5,
#endif
};

uint16_t OdeToJoy_Rhythm[] =
{
#ifdef REAL_ODE_TO_JOY
    QUARTER_NOTE,
    QUARTER_NOTE,
    QUARTER_NOTE,
    QUARTER_NOTE,
    QUARTER_NOTE,
    QUARTER_NOTE,
    QUARTER_NOTE,
    QUARTER_NOTE,
    HALF_NOTE,
    QUARTER_NOTE,
    QUARTER_NOTE,
    QUARTER_AND_HALF_NOTE,
    EIGHTH_NOTE,
    QUARTER_NOTE,
#else
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,

    250,
    250,
    250,
    250,
    250,
    250,
    250,
    250,
    500,
    250,
    250,
    250,
    250,
#endif
};

uint16_t Docked_Melody[] =
{
    NOTE_980_HZ,
    NOTE_1120_HZ,
    NOTE_980_HZ,
};

uint16_t Docked_Rhythm[] =
{
    40,
    40,
    40,
};

uint16_t KeyPress_Melody[] =
{
    NOTE_1800_HZ,
};

uint16_t KeyPress_Rhythm[] =
{
    40,
};

uint16_t Long_Melody[] =
{
    NOTE_2000_HZ,
};

uint16_t Long_Rhythm[] =
{
    900,
};

uint16_t Quick_Melody[] =
{
    NOTE_2000_HZ,
};

uint16_t Quick_Rhythm[] =
{
    150,
};

uint16_t QuickThree_Melody[] =
{
    NOTE_2000_HZ,
    REST_NOTE,
    NOTE_2000_HZ,
    REST_NOTE,
    NOTE_2000_HZ,
};

uint16_t QuickThree_Rhythm[] =
{
    150,
    600,
    150,
    600,
    150,
};

uint16_t Scan_Melody[] =
{
    NOTE_2500_HZ,
};

uint16_t Scan_Rhythm[] =
{
    100,
};

uint16_t Scale_Melody[] =
{
    NOTE_C4,
    NOTE_D4,
    NOTE_E4,
    NOTE_F4,
    NOTE_G4,
    NOTE_A4,
    NOTE_B4,
    NOTE_C5,
};

uint16_t Scale_Rhythm[] =
{
    300,
    300,
    300,
    300,
    300,
    300,
    300,
    300,
};

#if 0
/* Beethoven #5 */
uint16_t Nova_Melody[] =
{
    NOTE_G4,
    REST_NOTE,
    NOTE_G4,
    REST_NOTE,
    NOTE_G4,
    REST_NOTE,
    NOTE_E4,
};

uint16_t Nova_Rhythm[] =
{
    200,
    30,
    200,
    30,
    200,
    60,
    800,
};
#endif

#if 0
/* temp test Nova tune */
uint16_t Nova_Melody[] =
{
    NOTE_C4,
    REST_NOTE,
    NOTE_G4,
    REST_NOTE,
    NOTE_A4,
    REST_NOTE,
    NOTE_F4,
    REST_NOTE,
    NOTE_C5,
};


uint16_t Nova_Rhythm[] =
{
    TRIPLET_NOTE,
    NOTE_SEPARATION,
    TRIPLET_NOTE,
    NOTE_SEPARATION,
    TRIPLET_NOTE,
    TRIPLET_REST,
    EIGHTH_NOTE,
    NOTE_SEPARATION,
    QUARTER_NOTE,
};
#endif
uint16_t Nova_Melody[] =
{
    NOTE_A3,
    REST_NOTE,
    NOTE_C3,
    REST_NOTE,
    NOTE_E3,
    REST_NOTE,
    NOTE_G3,
    REST_NOTE,
};

uint16_t Nova_Rhythm[] =
{
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
    SIXTEENTH_NOTE,
    NOTE_SEPARATION,
};

#define BootTune_Melody OdeToJoy_Melody
#define BootTune_Rhythm OdeToJoy_Rhythm
