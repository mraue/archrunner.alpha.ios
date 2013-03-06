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
                                        IWPlayerData player,
                                        float radius)
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
        radius,
        IWTUTORIALSTAGE_STATUS_TEXT
    };
    return tutorialStage;
}

IWTutorialStageData IWTutorialStageMakeNonActionStage(char* title,
                                                      char* text)
{
    return IWTutorialStageMake(title, text, false, 0, 0, false, false, false, false, false,
                               IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                      IWVector3Make(0.0, 0.0, 1.0),
                                                      IWVector3Make(0.0, 1.0, 0.0)),
                               100.0);
}
