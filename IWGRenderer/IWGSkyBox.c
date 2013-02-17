//
//  IWGSkyBox.c
//  theBr1dge
//
//  Created by Martin Raue on 2/13/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGSkyBox.h"

#include <stdio.h>
#include <stdlib.h>

#include "IWUIDefinitions.h"

#include "IWGLighting.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWGSkyBoxData IWGSkyBoxMakeDefault()
{
    IWGSkyBoxData skyBox;
    
    skyBox.colorTransitionTime = 8.0 * 60.0;
    skyBox.transitionTime = 0.0;
    
    skyBox.skyColorDay = IWVector4Make(0.6, 0.6, 0.6, 1.0);
    skyBox.skyColorNight = IWUI_COLOR_BLUE(1.0);
    skyBox.sky = IWCubeMake(0, IWCUBE_TYPE_STANDARD,
                            IWVector3Make(0.0, 30.0, 0.0),
                            skyBox.skyColorDay,
                            IWVector3Make(70.0, 70.0, 70.0),
                            IWCUBE_FACES_LID,
                            IWCUBE_NORMALS_INWARD,
                            0.0, true, false, IWVector3TransitionMakeEmpty());
    skyBox.skyColorTransition = IWVector4TransitionMake(skyBox.skyColorDay,
                                                        skyBox.skyColorNight,
                                                        skyBox.skyColorDay,
                                                        skyBox.colorTransitionTime,
                                                        0.0, false, false);

    skyBox.groundColorDay = IWVector4Make(0.5, 0.5, 0.5, 1.0);
    skyBox.groundColorNight = IWUI_COLOR_DARK_BLUE(1.0);
    skyBox.ground = IWCubeMake(0, IWCUBE_TYPE_STANDARD,
                               IWVector3Make(0.0, -10.0, 0.0),
                               skyBox.groundColorDay,
                               IWVector3Make(60.0, 20.0, 60.0),
                               IWCUBE_FACES_BOWL,
                               IWCUBE_NORMALS_INWARD,
                               0.0, true, false, IWVector3TransitionMakeEmpty());
    skyBox.groundColorTransition = IWVector4TransitionMake(skyBox.groundColorDay,
                                                           skyBox.groundColorNight,
                                                           skyBox.groundColorDay,
                                                           skyBox.colorTransitionTime,
                                                           0.0, false, false);
    
    skyBox.sunColorDay = IWUI_COLOR_GOLD(1.0);
    skyBox.sunColorNight = IWUI_COLOR_DARK_RED(1.0);
    skyBox.sunPosition = IWVector3Make(0.0, -1.0, 30.5);
    skyBox.sun = IWGCircleMake(skyBox.sunPosition,
                               IWVector3Make(0.0, 0.0, 1.0),
                               skyBox.sunColorDay,
                               5.0, 31);
    skyBox.sunColorTransition = IWVector4TransitionMake(skyBox.sunColorDay,
                                                        skyBox.sunColorNight,
                                                        skyBox.sunColorDay,
                                                        skyBox.colorTransitionTime * 0.6,
                                                        0.0, false, false);
    
    skyBox.moonColorDay = IWVector4Make(0.65, 0.65, 0.65, 1.0);
    skyBox.moonColorNight = IWUI_COLOR_WHITE(1.0);
//    skyBox.moon = IWGCircleMake(IWVector3MultiplyScalar(IWVector3Normalize(IWVector3Make(1.0, 2.0, -1.0)), 30.5),
//                                IWVector3Normalize(IWVector3Make(1.0, 1.0, -1.0)),
//                                skyBox.moonColorDay,
//                                2.0, 41);
//    skyBox.moon = IWGCircleMake(IWVector3MultiplyScalar(IWVector3Normalize(IWVector3Make(1.0, 0.5, 1.0)), 30.5),
//                                IWVector3Normalize(IWVector3Make(1.0, 0.5, 1.0)),
//                                skyBox.moonColorDay,
//                                2.0, 41);
    skyBox.moonPosition = IWVector3MultiplyScalar(IWVector3Normalize(IWVector3Make(3.0, 2.5, -2.0)), 30.5);
    skyBox.moon = IWGCircleMake(skyBox.moonPosition,
                                IWVector3Normalize(IWVector3Make(3.0, 2.5, -2.0)),
                                skyBox.moonColorDay,
                                2.0, 31);
    skyBox.moonColorTransition = IWVector4TransitionMake(skyBox.moonColorDay,
                                                         skyBox.moonColorNight,
                                                         skyBox.moonColorDay,
                                                         skyBox.colorTransitionTime,
                                                         0.0, false, false);
    
    
    skyBox.dataBufferSize = (skyBox.sky.triangleBufferData.size
        + skyBox.ground.triangleBufferData.size
        + skyBox.sun.triangleBufferData.size
        + skyBox.moon.triangleBufferData.size);
    skyBox.dataBufferStart = malloc(skyBox.dataBufferSize * sizeof(GLfloat));
    
    size_t bufferOffset = 0;
    
    skyBox.sky.triangleBufferData.startCPU = skyBox.dataBufferStart + bufferOffset;
    bufferOffset += IWCubeToTriangles(&skyBox.sky);
    
    skyBox.ground.triangleBufferData.startCPU = skyBox.dataBufferStart + bufferOffset;
    bufferOffset += IWCubeToTriangles(&skyBox.ground);
    
    skyBox.sun.triangleBufferData.startCPU = skyBox.dataBufferStart + bufferOffset;
    bufferOffset += IWGCircleToTriangles(&skyBox.sun);
    
    skyBox.moon.triangleBufferData.startCPU = skyBox.dataBufferStart + bufferOffset;
    bufferOffset += IWGCircleToTriangles(&skyBox.moon);
    
    skyBox.multiBuffer = IWGMultiBufferGen();
    
    skyBox.mainShaderId = 4;// [4]

    return skyBox;
}

void IWGSkyBoxFillVBO(IWGSkyBoxData *skyBox, GLuint positionSlot,GLuint colorSlot, GLuint normalSlot)
{
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        IWGMultiBufferBind(&skyBox->multiBuffer, i);
        
        skyBox->multiBuffer.nVertices[i] = skyBox->dataBufferSize / 10;
        
        glBufferData(GL_ARRAY_BUFFER, skyBox->dataBufferSize * sizeof(GLfloat),
                     skyBox->dataBufferStart, GL_STREAM_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        //glEnableVertexAttribArray(normalSlot);
        //glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    glBindVertexArrayOES(0);
    return;
}

void IWGSkyBoxUpdate(IWGSkyBoxData *skyBox, float timeSinceLastUpdate, IWPlayerData *player, bool updateColor)
{
    skyBox->transitionTime += timeSinceLastUpdate;
    
    if (updateColor) {
        if (!skyBox->skyColorTransition.transitionHasFinished)
            IWVector4TransitionUpdate(&skyBox->skyColorTransition, timeSinceLastUpdate);
        if (!skyBox->groundColorTransition.transitionHasFinished)
            IWVector4TransitionUpdate(&skyBox->groundColorTransition, timeSinceLastUpdate);
        if (!skyBox->sunColorTransition.transitionHasFinished)
            IWVector4TransitionUpdate(&skyBox->sunColorTransition, timeSinceLastUpdate);
        if (!skyBox->moonColorTransition.transitionHasFinished)
            IWVector4TransitionUpdate(&skyBox->moonColorTransition, timeSinceLastUpdate);
    }
    
    IWGMultiBufferBindCurrentDataUpdateBuffer(&skyBox->multiBuffer);
    
    skyBox->sky.centerPosition.x = player->position.x;
    skyBox->sky.centerPosition.z = player->position.z;
    
    if (updateColor)
        skyBox->sky.color = skyBox->skyColorTransition.currentVector;
    
    IWCubeToTriangles(&skyBox->sky);
    
    skyBox->ground.centerPosition.x = player->position.x;
    skyBox->ground.centerPosition.z = player->position.z;
    
    if (updateColor)
        skyBox->ground.color = skyBox->groundColorTransition.currentVector;
    
    IWCubeToTriangles(&skyBox->ground);

    skyBox->sun.centerLocation.x = player->position.x + skyBox->sunPosition.x;
    skyBox->sun.centerLocation.z = player->position.z + skyBox->sunPosition.z;
    skyBox->sun.radius = (1. + player->position.z / 30.0) * 5.0;
    
    if (skyBox->sun.centerLocation.y > -10.0)
        skyBox->sun.centerLocation.y -= 5. * timeSinceLastUpdate / (skyBox->colorTransitionTime * 0.9);

    if (updateColor)
        skyBox->sun.color = skyBox->sunColorTransition.currentVector;
    
    IWGCircleToTriangles(&skyBox->sun);
    
    skyBox->moon.centerLocation.x = player->position.x + skyBox->moonPosition.x;
    skyBox->moon.centerLocation.z = player->position.z + skyBox->moonPosition.z;
    
    if (updateColor)
        skyBox->moon.color = skyBox->moonColorTransition.currentVector;
    
    IWGCircleToTriangles(&skyBox->moon);
    
    IWGMultiBufferSubData(&skyBox->multiBuffer,
                          0,
                          skyBox->dataBufferSize * sizeof(GLfloat),
                          skyBox->dataBufferStart,
                          false);
    

    return;
}

void IWGSkyBoxRender(IWGSkyBoxData *skyBox, bool setGLStates)
{
    IWGMultiBufferBindCurrentDrawBuffer(&skyBox->multiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, skyBox->multiBuffer.nVertices[skyBox->multiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGMultiBufferSwitchBuffer(&skyBox->multiBuffer);
}

void IWGSkyBoxPurgeData(IWGSkyBoxData *skyBox)
{
    IWGMultiBufferDealloc(&skyBox->multiBuffer);
    free(skyBox->dataBufferStart);
}
