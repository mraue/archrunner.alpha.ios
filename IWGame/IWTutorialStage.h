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

typedef struct {
    IWUSERINTERFACE_ELEMENT userInterfaceElements;
    unsigned int nCubesPerAxis;
    bool removeCubes;
    bool removeFuel;
    bool convertGridToBridge;
    char* title;
    char* text;
    
} IWTutorialStageData;

IWTutorialStageData IWTutorialStageMake();

#endif
