//
//  IWGSkyBox.h
//  theBr1dge
//
//  Created by Martin Raue on 2/13/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGSkyBox_h
#define theBr1dge_IWGSkyBox_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

#include "IWCube.h"
#include "IWGCircle.h"
#include "IWVectorTransition.h"

#include "IWPlayer.h"

#include "IWGMultiBuffer.h"

typedef struct {
    IWCubeData sky;
    IWCubeData ground;
    IWGCircleData sun;
    IWGCircleData moon;
    IWVector4 skyColorDay, skyColorNight;    
    IWVector4 groundColorDay, groundColorNight;
    IWVector4 sunColorDay, sunColorNight;
    float colorTransitionTime;
    IWVector4Transition skyColorTransition;
    IWVector4Transition sunColorTransition;
    IWVector4Transition groundColorTransition;
    size_t dataBufferSize;
    GLfloat* dataBufferStart;
    IWGMultiBufferData multiBuffer;
    unsigned int mainShaderId;
} IWGSkyBoxData;

IWGSkyBoxData IWGSkyBoxMakeDefault();

void IWGSkyBoxFillVBO(IWGSkyBoxData *skyBox, GLuint positionSlot,GLuint colorSlot, GLuint normalSlot);

void IWGSkyBoxUpdate(IWGSkyBoxData *skyBox, float timeSinceLastUpdate, IWPlayerData *player, bool updateColor);

void IWGSkyBoxRender(IWGSkyBoxData *skyBox, bool setGLStates);

void IWGSkyBoxPurgeData(IWGSkyBoxData *skyBox);

#endif
