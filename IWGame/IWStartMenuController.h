//
//  IWStartMenuController.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWStartMenuController_h
#define ArchRunnerAlpha_IWStartMenuController_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWVectorTransition.h"
#include "IWGeometry.h"
#include "IWGSkyBoxController.h"
#include "IWCubeController.h"
#include "IWTimer.h"
#include "IWGShaderProgram.h"
#include "IWGFontMap.h"
#include "IWUIMenuController.h"
#include "IWPlayer.h"
#include "IWGTextField.h"

typedef struct {
    float currentTransitionTime;
    //
    IWVector4 textColorText;
    IWTimerData transitionInteractionBlockTimer;
    float screenAspectRatio;
    IWGFontMapData *fontMap;
    //
    IWPlayerData player;
    //
    const IWGShaderProgramData *mainShaderProgram, *skyShaderProgram, *textShaderProgram;
    GLuint textureHandlerId;
    GLvoid* fontMapTextureData;
    //
    size_t textDataBufferSize;
    GLfloat* textDataBufferStart;
    IWGRingBufferData textMultiBuffer;
    //
    IWGTextFieldData versionTextField, titleTextField;
    //
    IWUIMenuControllerData menuController;
    IWGSkyBoxControllerData skyBoxController;
    IWCubeControllerData cubeController;
} IWStartMenuControllerData;


IWStartMenuControllerData* IWStartMenuControllerMakeDefault(float screenAspectRatio,
                                                            IWGFontMapData *fontMap);

void IWStartMenuControllerSetupVBOs(IWStartMenuControllerData *startMenuController,
                                    const IWGShaderProgramData *mainShaderProgram,
                                    const IWGShaderProgramData *skyShaderProgram,
                                    const IWGShaderProgramData *textShaderProgram,
                                    GLuint textureHandlerId,
                                    GLvoid* fontMapTextureData);

void IWStartMenuControllerUpdate(IWStartMenuControllerData *startMenuController,
                                 IWPoint2D touchPoint,
                                 bool *isTouched,
                                 float timeSinceLastUpdate);

void IWStartMenuControllerRender(IWStartMenuControllerData *startMenuController,
                                 const IWGShaderProgramData *mainShaderProgram,
                                 const IWGShaderProgramData *uiShaderProgram,
                                 const IWGShaderProgramData *textShaderProgram);

void IWStartMenuControllerPurgeData(IWStartMenuControllerData *startMenuController);

#endif
