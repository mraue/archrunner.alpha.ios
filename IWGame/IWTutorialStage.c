//
//  IWTutorialStage.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWTutorialStage.h"

#include <stdio.h>


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
                                        IWPlayerData player)
{
    IWTutorialStageData tutorialStage = {
        title,
        text,
        hasActionStage,
        userInterfaceElements,
        nCubesPerAxis,
        cubesInteractive,
        removeCubes,
        removeFuel,
        convertGridToBridge,
        activateOverdrive,
        player,
        IWTUTORIALSTAGE_STATUS_TEXT
    };
    return tutorialStage;
}
