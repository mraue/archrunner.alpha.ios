//
//  IWTutorialController.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWTutorialController_h
#define ArchRunnerAlpha_IWTutorialController_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWVectorTransition.h"
#include "IWGeometry.h"
#include "IWGTextField.h"
#include "IWGSkyBoxController.h"
#include "IWUserInterfaceController.h"
#include "IWTutorialStage.h"
#include "IWCubeController.h"
#include "IWTimer.h"
#include "IWGShaderProgram.h"
#include "IWScoreCounter.h"
#include "IWCubeStatus.h"
#include "IWFuel.h"
#include "IWPlayer.h"
#include "IWGFontMap.h"
#include "IWController.h"

typedef struct {
    //
    float tutorialRadius;
    //
    IWTutorialStageData stages[8];
    unsigned int currentStage;
    unsigned int nStages;
    bool hasFinished;
    //
    IWVector3Transition grayScaleTransition, grayScaleTransitionBase;
    //
    IWVector4 textColorText, textColorRunning;
    IWTimerData transitionInteractionBlockTimer;
    float screenAspectRatio;
    IWGFontMapData *fontMap;
    //
    const IWGShaderProgramData *mainShaderProgram, *skyShaderProgram, *uiShaderProgram, *textShaderProgram;
    GLuint textureHandlerId;
    GLvoid* fontMapTextureData;
    //
    IWGTextFieldData skipTutorialButton, startNextButton, stageTitle, stageText;
    IWRectangle skipTutorialButtonRectangle, startNextButtonRectangle;
    size_t textDataBufferSize;
    GLfloat* textDataBufferStart;
    IWGRingBufferData textMultiBuffer;
    //
    IWGSkyBoxControllerData skyBoxController;
    IWUserInterfaceControllerData userInterfaceController;
    IWCubeControllerData cubeController;
} IWTutorialControllerData;


IWTutorialControllerData* IWTutorialControllerMakeDefault(float screenAspectRatio,
                                                          IWGFontMapData *fontMap);

void IWTutorialControllerSetupVBOs(IWTutorialControllerData *tutorialController,
                                   const IWGShaderProgramData *mainShaderProgram,
                                   const IWGShaderProgramData *skyShaderProgram,
                                   const IWGShaderProgramData *uiShaderProgram,
                                   const IWGShaderProgramData *textShaderProgram,
                                   GLuint textureHandlerId,
                                   GLvoid* fontMapTextureData);

void IWTutorialControllerUpdate(IWTutorialControllerData *tutorialController,
                                IWScoreCounterData *scoreCounter,
                                IWCubeStatusData *cubeStatus,
                                IWFuel *fuel,
                                IWPlayerData *player,
                                IWPoint2D touchPoint,
                                bool *isTouched,
                                IWControllerData *controller,
                                float timeSinceLastUpdate);

void IWTutorialControllerRender(IWTutorialControllerData *tutorialController,
                                const IWGShaderProgramData *mainShaderProgram,
                                const IWGShaderProgramData *skyShaderProgram,
                                const IWGShaderProgramData *uiShaderProgram,
                                const IWGShaderProgramData *textShaderProgram);

void IWTutorialControllerPurgeData(IWTutorialControllerData *tutorialController);

#endif
