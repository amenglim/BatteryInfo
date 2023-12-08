// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//                         COPYRIGHT NOTICE
//                   "Copyright 2021 Nova Biomedical Corporation"
//             This program is the property of Nova Biomedical Corporation
//                 200 Prospect Street, Waltham, MA 02454-9141
//             Any unauthorized use or duplication is prohibited
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
/// Title            -  Supported Audio Track IDs
/// Source Filename  -  AudioTrackID.h
/// Originator       -  CWong
/// Description      -  An enumeration of Track play-list
///
/// Dependencies     -  <none>
//
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

typedef enum
{
    Scale,
    Alarm1,
    Alarm2,
    Alert1,
    Alert2,
    BadInput,
    BootTune,
    Docked,
    KeyPress,
    Long,
    Quick,
    QuickThree,
    Scan,
    Nova,
    MaxTracks,      /* end of list */
} AudioTrackID;
