//
//  IWTutorialStage.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWTutorialStage_h
#define ArchRunnerAlpha_IWTutorialStage_h

#include <stdbool.h>

#include "IWUserInterfaceController.h"
#include "IWPlayer.h"

typedef enum {
    IWTUTORIALSTAGE_STATUS_RUNNING,
    IWTUTORIALSTAGE_STATUS_TEXT,
    IWTUTORIALSTAGE_STATUS_PAUSED
} IWTUTORIALSTAGE_STATUS;

typedef struct {
    char* title;
    char* text;
    bool hasActionStage;
    IWUSERINTERFACE_ELEMENT userInterfaceElements;
    unsigned int nCubesPerAxis;
    bool cubesInteractive;
    bool removeCubes;
    bool removeFuel;
    bool convertGridToBridge;
    bool activateOverdrive;
    IWPlayerData player;    
    IWTUTORIALSTAGE_STATUS status;
} IWTutorialStageData;

IWTutorialStageData IWTutorialStageMake(char* title,
                                        char* text,
                                        bool hasActionStage,
                                        IWUSERINTERFACE_ELEMENT userInterfaceElements,
                                        unsigned int nCubesPerAxis,
                                        bool cubesInteractive,
                                        bool removeCubes,
                                        bool removeFuel,
                                        bool convertGridToBridge,
                                        bool activateOverdrive,
                                        IWPlayerData player);

#endif
