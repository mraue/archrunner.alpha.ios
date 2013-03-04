//
//  IWTutorialController.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWTutorialController_h
#define ArchRunnerAlpha_IWTutorialController_h

#include "IWMath.h"
#include "IWGeometry.h"
#include "IWGTextField.h"
#include "IWGSkyBoxController.h"
#include "IWUserInterfaceController.h"
#include "IWTutorialStage.h"

typedef struct {
    IWTutorialStageData stages[1];
    unsigned int currentStage;
    unsigned int nStages;
    IWGSkyBoxControllerData skyBoxController;
    IWUserInterfaceControllerData userInterface;
} IWTutorialControllerData;

#endif
