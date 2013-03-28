//
//  IWGSkyBoxController.h
//  theBr1dge
//
//  Created by Martin Raue on 2/13/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGSkyBoxController_h
#define theBr1dge_IWGSkyBoxController_h

#include <stdbool.h>

#include "IWOpenGL.h"

#include "IWMath.h"

#include "IWCube.h"
#include "IWGCircle.h"
#include "IWVectorTransition.h"

#include "IWPlayer.h"

#include "IWGRingBuffer.h"
#include "IWGShaderProgram.h"

typedef struct {
    IWCubeData sky;
    IWCubeData ground;
    IWGCircleData sun;
    IWGCircleData moon;
    IWVector4 skyColorDay, skyColorNight;    
    IWVector4 groundColorDay, groundColorNight;
    IWVector4 sunColorDay, sunColorNight, sunColorLight;
    IWVector4 moonColorDay, moonColorNight;
    IWVector3 moonPosition, sunPosition;
    float colorTransitionTime;
    float transitionTime;
    IWVector4Transition skyColorTransition;
    IWVector4Transition groundColorTransition;
    IWVector4Transition sunColorTransition;
    IWVector4Transition moonColorTransition;
    size_t dataBufferSize;
    GLfloat* dataBufferStart;
    IWGRingBufferData multiBuffer;
    const IWGShaderProgramData *skyShaderProgram;
} IWGSkyBoxControllerData;

IWGSkyBoxControllerData IWGSkyBoxControllerMakeDefault();

void IWGSkyBoxControllerFillVBO(IWGSkyBoxControllerData *skyBoxController,
                                const IWGShaderProgramData *shaderProgram);

void IWGSkyBoxControllerUpdate(IWGSkyBoxControllerData *skyBoxController,
                               float timeSinceLastUpdate,
                               const IWPlayerData *player,
                               bool updateColor);

void IWGSkyBoxControllerRender(IWGSkyBoxControllerData *skyBoxController);

void IWGSkyBoxControllerPurgeData(IWGSkyBoxControllerData *skyBoxController);

#endif
