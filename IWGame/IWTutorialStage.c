//
//  IWTutorialStage.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWTutorialStage.h"

#include <stdio.h>


IWTutorialStageData IWTutorialStageMake(IWUSERINTERFACE_ELEMENT userInterfaceElements,
                                        unsigned int nCubesPerAxis,
                                        bool removeCubes,
                                        bool removeFuel,
                                        bool convertGridToBridge,
                                        char* title,
                                        char* text,
                                        IWPlayerData player)
{
    IWTutorialStageData tutorialStage = {
        userInterfaceElements,
        nCubesPerAxis,
        removeCubes,
        removeFuel,
        convertGridToBridge,
        title,
        text,
        player,
        IWTUTORIALSTAGE_STATUS_TEXT
    };
    return tutorialStage;
}
