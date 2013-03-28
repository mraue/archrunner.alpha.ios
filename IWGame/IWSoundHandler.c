//
//  IWSoundHandler.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/12/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWSoundHandler.h"

#include <stdio.h>
#include <stdlib.h>

IWSoundHandlerData* IWSoundHandlerMake(unsigned int nSoundsMax)
{
    IWSoundHandlerData* soundHandler = (IWSoundHandlerData*)malloc(sizeof(IWSoundHandlerData));
    soundHandler->nSoundsMax = nSoundsMax;
    soundHandler->nSounds = 0;
    soundHandler->sounds = malloc(sizeof(int) * nSoundsMax);
    return soundHandler;
}

void IWSoundHandlerAddSound(IWSoundHandlerData* soundHandler,
                            IWSOUNDHANDLER_SOUNDS sound)
{
    if (soundHandler->nSounds >= soundHandler->nSoundsMax) {
        printf("ERROR IWSoundHandlerAddSound: Sound array is full\n");
        return;
    }
    
    soundHandler->sounds[soundHandler->nSounds] = sound;
    soundHandler->nSounds++;
}

void IWSoundHandlerEmtpyAndReset(IWSoundHandlerData* soundHandler)
{
    soundHandler->nSounds = 0;
}

void IWSoundHandlerPurgeData(IWSoundHandlerData* soundHandler)
{
    free(soundHandler->sounds);
    soundHandler->sounds = NULL;
    return;
}