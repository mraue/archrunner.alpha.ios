//
//  IWSoundHandler.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/12/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWSoundHandler_h
#define ArchRunnerAlpha_IWSoundHandler_h

typedef enum {
    IWSOUNDHANDLER_SOUNDS_MENU_SELECTED,
    IWSOUNDHANDLER_SOUNDS_GREY_CUBE_TOUCHED,
    IWSOUNDHANDLER_SOUNDS_RED_CUBE_TOUCHED,
    IWSOUNDHANDLER_SOUNDS_CUBES_SPAWNING,
    IWSOUNDHANDLER_SOUNDS_OVERDRIVE,
    IWSOUNDHANDLER_SOUNDS_FUEL_WARNING,
    IWSOUNDHANDLER_SOUNDS_N
} IWSOUNDHANDLER_SOUNDS;

typedef struct {
    unsigned int nSounds;
    int *sounds;
    unsigned int nSoundsMax;
} IWSoundHandlerData;

IWSoundHandlerData* IWSoundHandlerMake(unsigned int nSoundsMax);

void IWSoundHandlerAddSound(IWSoundHandlerData* soundHandler,
                            IWSOUNDHANDLER_SOUNDS sound);

void IWSoundHandlerEmtpyAndReset(IWSoundHandlerData* soundHandler);

void IWSoundHandlerPurgeData(IWSoundHandlerData* soundHandler);


#endif
