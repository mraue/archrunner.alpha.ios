//
//  IWGSkyBoxController.c
//  theBr1dge
//
//  Created by Martin Raue on 2/13/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGSkyBoxController.h"

#include <stdio.h>
#include <stdlib.h>

#include "IWUIDefinitions.h"

#include "IWGLighting.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWGSkyBoxControllerData IWGSkyBoxControllerMakeDefault()
{
    IWGSkyBoxControllerData skyBoxController;
    
    skyBoxController.colorTransitionTime = 8.0 * 60.0;
    skyBoxController.transitionTime = 0.0;
    
    skyBoxController.skyColorDay = IWVector4Make(0.6, 0.6, 0.6, 1.0);
    skyBoxController.skyColorNight = IWUI_COLOR_BLUE(1.0);
    skyBoxController.sky = IWCubeMake(0, IWCUBE_TYPE_STANDARD,
                                      IWVector3Make(0.0, 30.0, 0.0),
                                      skyBoxController.skyColorDay,
                                      IWVector3Make(70.0, 70.0, 70.0),
                                      IWCUBE_FACES_LID,
                                      IWCUBE_NORMALS_INWARD,
                                      0.0, true, false, IWVector3TransitionMakeEmpty());
    skyBoxController.skyColorTransition = IWVector4TransitionMake(skyBoxController.skyColorDay,
                                                                  skyBoxController.skyColorNight,
                                                                  skyBoxController.skyColorDay,
                                                                  skyBoxController.colorTransitionTime,
                                                                  0.0, false, false);
    
    skyBoxController.groundColorDay = IWVector4Make(0.5, 0.5, 0.5, 1.0);
    skyBoxController.groundColorNight = IWUI_COLOR_DARK_BLUE(1.0);
    skyBoxController.ground = IWCubeMake(0, IWCUBE_TYPE_STANDARD,
                                         IWVector3Make(0.0, -10.0, 0.0),
                                         skyBoxController.groundColorDay,
                                         IWVector3Make(55.0, 20.0, 55.0),
                                         IWCUBE_FACES_BOWL,
                                         IWCUBE_NORMALS_INWARD,
                                         0.0, true, false, IWVector3TransitionMakeEmpty());
    skyBoxController.groundColorTransition = IWVector4TransitionMake(skyBoxController.groundColorDay,
                                                                     skyBoxController.groundColorNight,
                                                                     skyBoxController.groundColorDay,
                                                                     skyBoxController.colorTransitionTime,
                                                                     0.0, false, false);
    
    skyBoxController.sunColorDay = IWUI_COLOR_GOLD(1.0);
    skyBoxController.sunColorNight = IWUI_COLOR_DARK_RED(1.0);
    skyBoxController.sunPosition = IWVector3Make(0.0, -1.0, 30.5);
    skyBoxController.sun = IWGCircleMake(skyBoxController.sunPosition,
                                         IWVector3Make(0.0, 0.0, 1.0),
                                         skyBoxController.sunColorDay,
                                         5.0, 31);
    skyBoxController.sunColorTransition = IWVector4TransitionMake(skyBoxController.sunColorDay,
                                                                  skyBoxController.sunColorNight,
                                                                  skyBoxController.sunColorDay,
                                                                  skyBoxController.colorTransitionTime * 0.6,
                                                                  0.0, false, false);
    skyBoxController.sunColorLight = IWVector4Lerp(skyBoxController.sunColorDay,
                                                   IWVector4Make(1.0, 1.0, 1.0, 1.0),
                                                   0.85);
    
    skyBoxController.moonColorDay = IWVector4Make(0.65, 0.65, 0.65, 1.0);
    skyBoxController.moonColorNight = IWUI_COLOR_WHITE(1.0);
    //    skyBoxController.moon = IWGCircleMake(IWVector3MultiplyScalar(IWVector3Normalize(IWVector3Make(1.0, 2.0, -1.0)), 30.5),
    //                                IWVector3Normalize(IWVector3Make(1.0, 1.0, -1.0)),
    //                                skyBoxController.moonColorDay,
    //                                2.0, 41);
    //    skyBoxController.moon = IWGCircleMake(IWVector3MultiplyScalar(IWVector3Normalize(IWVector3Make(1.0, 0.5, 1.0)), 30.5),
    //                                IWVector3Normalize(IWVector3Make(1.0, 0.5, 1.0)),
    //                                skyBoxController.moonColorDay,
    //                                2.0, 41);
    skyBoxController.moonPosition = IWVector3MultiplyScalar(IWVector3Normalize(IWVector3Make(3.0, 2.5, -2.0)), 30.5);
    skyBoxController.moon = IWGCircleMake(skyBoxController.moonPosition,
                                          IWVector3Normalize(IWVector3Make(3.0, 2.5, -2.0)),
                                          skyBoxController.moonColorDay,
                                          2.0, 31);
    skyBoxController.moonColorTransition = IWVector4TransitionMake(skyBoxController.moonColorDay,
                                                                   skyBoxController.moonColorNight,
                                                                   skyBoxController.moonColorDay,
                                                                   skyBoxController.colorTransitionTime,
                                                                   0.0, false, false);
    
    
    skyBoxController.dataBufferSize = (skyBoxController.sky.triangleBufferData.size
                                       + skyBoxController.ground.triangleBufferData.size
                                       + skyBoxController.sun.triangleBufferData.size
                                       + skyBoxController.moon.triangleBufferData.size);
    skyBoxController.dataBufferStart = malloc(skyBoxController.dataBufferSize * sizeof(GLfloat));
    
    size_t bufferOffset = 0;
    
    skyBoxController.sky.triangleBufferData.startCPU = skyBoxController.dataBufferStart + bufferOffset;
    bufferOffset += IWCubeToTriangles(&skyBoxController.sky);
    
    skyBoxController.ground.triangleBufferData.startCPU = skyBoxController.dataBufferStart + bufferOffset;
    bufferOffset += IWCubeToTriangles(&skyBoxController.ground);
    
    skyBoxController.sun.triangleBufferData.startCPU = skyBoxController.dataBufferStart + bufferOffset;
    bufferOffset += IWGCircleToTriangles(&skyBoxController.sun);
    
    skyBoxController.moon.triangleBufferData.startCPU = skyBoxController.dataBufferStart + bufferOffset;
    bufferOffset += IWGCircleToTriangles(&skyBoxController.moon);
    
    skyBoxController.multiBuffer = IWGRingBufferGen();
    
    return skyBoxController;
}

void IWGSkyBoxControllerFillVBO(IWGSkyBoxControllerData *skyBoxController, GLuint positionSlot,GLuint colorSlot, GLuint normalSlot)
{
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        IWGRingBufferBind(&skyBoxController->multiBuffer, i);
        
        skyBoxController->multiBuffer.nVertices[i] = skyBoxController->dataBufferSize / 10;
        
        glBufferData(GL_ARRAY_BUFFER, skyBoxController->dataBufferSize * sizeof(GLfloat),
                     skyBoxController->dataBufferStart, GL_STREAM_DRAW);
        
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

void IWGSkyBoxControllerUpdate(IWGSkyBoxControllerData *skyBoxController, float timeSinceLastUpdate, IWPlayerData *player, bool updateColor)
{
    skyBoxController->transitionTime += timeSinceLastUpdate;
    
    //    if (updateColor) {
    //        if (!skyBoxController->skyColorTransition.transitionHasFinished)
    //            IWVector4TransitionUpdate(&skyBoxController->skyColorTransition, timeSinceLastUpdate);
    //        if (!skyBoxController->groundColorTransition.transitionHasFinished)
    //            IWVector4TransitionUpdate(&skyBoxController->groundColorTransition, timeSinceLastUpdate);
    //        if (!skyBoxController->sunColorTransition.transitionHasFinished)
    //            IWVector4TransitionUpdate(&skyBoxController->sunColorTransition, timeSinceLastUpdate);
    //        if (!skyBoxController->moonColorTransition.transitionHasFinished)
    //            IWVector4TransitionUpdate(&skyBoxController->moonColorTransition, timeSinceLastUpdate);
    //    }
    
    if (updateColor) {
        skyBoxController->skyColorTransition.currentTransitionTime = skyBoxController->transitionTime;
        IWVector4TransitionSet(&skyBoxController->skyColorTransition);
        skyBoxController->groundColorTransition.currentTransitionTime = skyBoxController->transitionTime;
        IWVector4TransitionSet(&skyBoxController->groundColorTransition);
        skyBoxController->sunColorTransition.currentTransitionTime = skyBoxController->transitionTime;
        IWVector4TransitionSet(&skyBoxController->sunColorTransition);
        skyBoxController->moonColorTransition.currentTransitionTime = skyBoxController->transitionTime;
        IWVector4TransitionSet(&skyBoxController->moonColorTransition);
    }
    
    IWGRingBufferBindCurrentDataUpdateBuffer(&skyBoxController->multiBuffer);
    
    skyBoxController->sky.centerPosition.x = player->position.x;
    skyBoxController->sky.centerPosition.z = player->position.z;
    
    if (updateColor)
        skyBoxController->sky.color = skyBoxController->skyColorTransition.currentVector;
    
    IWCubeToTriangles(&skyBoxController->sky);
    
    skyBoxController->ground.centerPosition.x = player->position.x;
    skyBoxController->ground.centerPosition.z = player->position.z;
    
    if (updateColor)
        skyBoxController->ground.color = skyBoxController->groundColorTransition.currentVector;
    
    IWCubeToTriangles(&skyBoxController->ground);
    
    skyBoxController->sun.centerLocation.x = player->position.x + skyBoxController->sunPosition.x;
    skyBoxController->sun.centerLocation.z = player->position.z + skyBoxController->sunPosition.z;
    skyBoxController->sun.radius = (1. + player->position.z / 30.0) * 5.0;
    
    //    if (skyBoxController->sun.centerLocation.y > -10.0)
    //        skyBoxController->sun.centerLocation.y -= 5. * timeSinceLastUpdate / (skyBoxController->colorTransitionTime * 0.9);
    
    float sunY = -1.0 - 5.0 * skyBoxController->transitionTime / (skyBoxController->colorTransitionTime * 0.9);
    skyBoxController->sun.centerLocation.y = sunY < -10.0 ? -10.0 : sunY;
    
    if (updateColor)
        skyBoxController->sun.color = skyBoxController->sunColorTransition.currentVector;
    
    skyBoxController->sunColorLight = IWVector4Lerp(skyBoxController->sunColorTransition.currentVector,
                                                    IWVector4Make(1.0, 1.0, 1.0, 1.0),
                                                    0.85);
    
    skyBoxController->sunColorLight.w = IW_MAX(0.0, 1.0 - skyBoxController->transitionTime / (skyBoxController->colorTransitionTime * 1.1));
    
    IWGCircleToTriangles(&skyBoxController->sun);
    
    skyBoxController->moon.centerLocation.x = player->position.x + skyBoxController->moonPosition.x;
    skyBoxController->moon.centerLocation.z = player->position.z + skyBoxController->moonPosition.z;
    
    if (updateColor)
        skyBoxController->moon.color = skyBoxController->moonColorTransition.currentVector;
    
    IWGCircleToTriangles(&skyBoxController->moon);
    
    IWGRingBufferSubData(&skyBoxController->multiBuffer,
                         0,
                         skyBoxController->dataBufferSize * sizeof(GLfloat),
                         skyBoxController->dataBufferStart,
                         false);
    
    
    return;
}

void IWGSkyBoxControllerRender(IWGSkyBoxControllerData *skyBoxController, bool setGLStates)
{
    IWGRingBufferBindCurrentDrawBuffer(&skyBoxController->multiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, skyBoxController->multiBuffer.nVertices[skyBoxController->multiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGRingBufferSwitchBuffer(&skyBoxController->multiBuffer);
}

void IWGSkyBoxControllerPurgeData(IWGSkyBoxControllerData *skyBoxController)
{
    IWGRingBufferDealloc(&skyBoxController->multiBuffer);
    free(skyBoxController->dataBufferStart);
}
