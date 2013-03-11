//
//  IWUserInterfaceController.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/28/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWUserInterfaceController_h
#define ArchRunnerAlpha_IWUserInterfaceController_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

#include "IWGRingBuffer.h"
#include "IWGShaderProgram.h"
#include "IWGTextField.h"
#include "IWGFontMap.h"
#include "IWUIRectangleButton.h"
#include "IWUIStateBar.h"
#include "IWScoreCounter.h"
#include "IWCubeStatus.h"
#include "IWVectorTransition.h"
#include "IWFuel.h"
#include "IWPlayer.h"

typedef enum {
    IWUSERINTERFACE_ELEMENT_SCORE = 1 <<  0,
    IWUSERINTERFACE_ELEMENT_FUEL_STATUS_BAR = 1 <<  1,
    IWUSERINTERFACE_ELEMENT_PAUSE_BUTTON = 1 <<  2,
    IWUSERINTERFACE_ELEMENT_CUBE_COUNTER = 1 <<  3,
    IWUSERINTERFACE_ELEMENT_HUD = 1 <<  4,
    IWUSERINTERFACE_ELEMENT_ALL = (1 << 5) - 1
} IWUSERINTERFACE_ELEMENT;

typedef struct {
    //
    IWUSERINTERFACE_ELEMENT visibleElements;
    //
    size_t triangleDataBufferSize;
    GLfloat* triangleDataBufferStart;
    IWGRingBufferData triangleMultiBuffer;
    size_t textDataBufferSize;
    GLfloat* textDataBufferStart;
    IWGRingBufferData textMultiBuffer;
    size_t lineDataBufferSize;
    GLfloat* lineDataBufferStart;
    IWGRingBufferData lineMultiBuffer;
    //
    IWGTextFieldData scoreTextField, cubeStatusTextField1, cubeStatusTextField2, fuelStatusTextField;
    //
    IWUIRectangleButton pauseButton;
    //
    IWUIStateBar fuelStateBar;
    //
    IWVector4Transition poolCubesColorTransition, fuelColorTransitionOverdrive, fuelColorTransitionWarning;
} IWUserInterfaceControllerData;

IWUserInterfaceControllerData IWUserInterfaceControllerMake(float screenAspectRatio,
                                                            IWUSERINTERFACE_ELEMENT visibleElements,
                                                            IWGFontMapData *fontMap);

void IWUserInterfaceControllerSetupVBOs(IWUserInterfaceControllerData *userInterfaceController,
                                        const IWGShaderProgramData *uiShaderProgram,
                                        const IWGShaderProgramData *textShaderProgram,
                                        GLuint textureHandlerId,
                                        GLvoid* fontMapTextureData);

void IWUserInterfaceControllerUpdate(IWUserInterfaceControllerData *userInterfaceController,
                                     const IWScoreCounterData *scoreCounter,
                                     const IWCubeStatusData *cubeStatus,
                                     const IWFuel *fuel,
                                     const IWPlayerData *player,
                                     float timeSinceLastUpdate);

void IWUserInterfaceControllerRender(IWUserInterfaceControllerData *userInterfaceController,
                                     GLuint textProgramId,
                                     GLuint uiProgramId);

void IWUserInterfacePurgeData(IWUserInterfaceControllerData *userInterfaceController);

#endif
