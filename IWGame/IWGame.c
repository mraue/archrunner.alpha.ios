//
//  IWGame.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGame.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWController.h"

#include "IWGBufferSubData.h"
#include "IWGDoubleBuffer.h"

#include "IWGameData.h"

void IWGameSetup(void)
{
    gdPlayerData = IWPlayerDataMake(IWVector3Make(1.4, 0.8, 1.4),
                                    IWVector3Normalize(IWVector3Make(-1.0, 0.0, -1.0)),
                                    IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0)));
    gdTotalRunTime = 0.0;
    return;
}

void IWGameUpdate(float timeSinceLastUpdate)
{
    gdTotalRunTime += timeSinceLastUpdate;
    
    // Switch draw buffer
    IWGDoubleBufferSwitchBuffer(&gdTriangleDoubleBuffer);
    
    // Update fuel
    IWFuelRemoveFuel(&gdFuel, 0.05 * timeSinceLastUpdate);
    
    for (int i = 0; i < gdNCubes; i++) {
        if (gdCubeData[i].isVisible && gdCubeData[i].collisionRadius > 0.0) {
            if (IWVector3DistanceSquared(gdPlayerData.position, gdCubeData[i].centerPosition)
                < gdCubeData[i].collisionRadius * gdCubeData[i].collisionRadius) {
                // Collision detected
                //
                //IWGPrimitiveBufferDataUpdateColor(&gdCubeData[i].triangleBufferData, IWUI_COLOR_DARK_BLUE(1.0));
                gdCubeData[i].centerPosition = IWVector3Add(gdCubeData[i].centerPosition,
                                                            IWVector3Make(0.0, 0.2, 0.0));
                IWCubeToTriangles(&gdCubeData[i]);
                
                IWGDoubleBufferSubData(&gdTriangleDoubleBuffer,
                                       gdCubeData[i].triangleBufferData.size * i * sizeof(GLfloat),
                                       gdCubeData[i].triangleBufferData.size * sizeof(GLfloat),
                                       gdCubeData[i].triangleBufferData.startCPU,
                                       false);
                gdFuel.currentLevel = gdFuel.currentMaxLevel;
                // Hide cube
                gdCubeData[i].isVisible = false;
//                if (gdBufferToCubeMapNEntries > 0) {
//                    
//                    gdBufferToCubeMapNEntries -= 1;
//                    unsigned currentBufferID, newCubeID;
//                    //currentBufferID = gdCubeToBufferMap[i];
//                    currentBufferID = gdCubeData[i].triangleBufferData.bufferIDGPU;
//                    newCubeID = gdBufferToCubeMap[gdBufferToCubeMapNEntries];
//                    
//                    if (newCubeID != i) {
//                        gdBufferToCubeMap[currentBufferID] = newCubeID;
//                        //gdCubeToBufferMap[newCubeID] = currentBufferID;
//                        gdCubeData[newCubeID].triangleBufferData.bufferIDGPU = currentBufferID;
//
//                        IWGDoubleBufferSubData(&gdTriangleDoubleBuffer,
//                                               gdCubeData[i].triangleBufferData.size * currentBufferID * sizeof(GLfloat),
//                                               gdCubeData[i].triangleBufferData.size * sizeof(GLfloat),
//                                               gdCubeData[newCubeID].triangleBufferData.startCPU,
//                                               false);
//                    }
//                    
//                    printf("BOING %u %u %u\n", i, currentBufferID, newCubeID);
//                    gdFuel.currentLevel = gdFuel.currentMaxLevel;
//                }
//                //glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
//                gdB2TriangleNVertices = gdB1TriangleNVertices -= gdCubeData[i].triangleBufferData.size / gdCubeData[i].triangleBufferData.stride;
//                gdTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentBuffer] = gdB1TriangleNVertices;
                
                gdClearColorTransition.currentTransitionTime = 0.0;
                gdClearColorTransition.transitionHasFinished = false;
            }
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    IWGDoubleBufferSwitchBuffer(&gdUITriangleDoubleBuffer);
    
    if (!gdClearColorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdClearColorTransition, timeSinceLastUpdate);
    }
    
    if (gdFuel.currentLevel == 0.0) {
        gdPlayerData.position = IWVector3Make(1.4, 0.8, 1.4);
        gdPlayerData.direction = IWVector3Normalize(IWVector3Make(-1.0, 0.0, -1.0));
        gdPlayerData.up = IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0));
        gdFuel.currentLevel = gdFuel.currentMaxLevel;
    } else if (gdFuel.currentLevel / gdFuel.currentMaxLevel < 0.333) {
        if (!gdFuel.isWarning) {
            IWFuelUpdateColor(&gdFuel, gdFuel.warningColor, IWFUEL_COLOR_CURRENT, false);
            gdFuel.isWarning = true;
        }
    } else {
        IWFuelUpdateColor(&gdFuel, gdFuel.currentColor, IWFUEL_COLOR_CURRENT, false);
        gdFuel.isWarning = false;
    }
    
    glBindVertexArrayOES(gdUITriangleVertexArray);
    
    // Fuel vertex update
    IWFuelToTriangleBuffer(&gdFuel, gdFuel.stateBar.uiElementData.triangleBufferStart);
    
    IWGDoubleBufferSubData(&gdUITriangleDoubleBuffer,
                           (gdFuel.stateBar.uiElementData.triangleBufferStart - gdRectangleButton.triangleBufferStart)  * sizeof(GLfloat),
                           gdFuel.stateBar.uiElementData.triangleBufferSize * sizeof(GLfloat),
                           gdFuel.stateBar.uiElementData.triangleBufferStart,
                           false);
    
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton, gdIsTouched, gdTouchPoint)) {
        if (gdDropCamera) {
            gdDropCamera = false;
        } else {
            gdDropCamera = true;
            gdPlayerDataSave = gdPlayerData;
        }
    }
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton2, gdIsTouched, gdTouchPoint)) {
        gdPlayerData.position = IWVector3Make(1.6, 1.0, 1.6);
        gdPlayerData.direction = IWVector3Normalize(IWVector3Make(-1.0, 0.0, -1.0));
        gdPlayerData.up = IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0));
    }
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton3, gdIsTouched, gdTouchPoint)) {
        gdResetControllerPosition = true;
    }
    
    if (!gdRectangleButton.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton.colorTransition, timeSinceLastUpdate);
        gdRectangleButton.color = gdRectangleButton.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);
        IWGDoubleBufferSubData(&gdUITriangleDoubleBuffer,
                               0,
                               gdRectangleButton.triangleBufferSize * sizeof(GLfloat),
                               gdRectangleButton.triangleBufferStart,
                               false);
    }
    if (!gdRectangleButton2.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton2.colorTransition, timeSinceLastUpdate);
        gdRectangleButton2.color = gdRectangleButton2.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton2);
        IWGDoubleBufferSubData(&gdUITriangleDoubleBuffer,
                               (gdRectangleButton2.triangleBufferStart - gdRectangleButton.triangleBufferStart) * sizeof(GLfloat),
                               gdRectangleButton2.triangleBufferSize * sizeof(GLfloat),
                               gdRectangleButton2.triangleBufferStart,
                               false);
    }
    if (!gdRectangleButton3.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton3.colorTransition, timeSinceLastUpdate);
        gdRectangleButton3.color = gdRectangleButton3.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton3);
        IWGDoubleBufferSubData(&gdUITriangleDoubleBuffer,
                               (gdRectangleButton3.triangleBufferStart - gdRectangleButton.triangleBufferStart)  * sizeof(GLfloat),
                               gdRectangleButton3.triangleBufferSize * sizeof(GLfloat),
                               gdRectangleButton3.triangleBufferStart,
                               false);
    }
    glBindVertexArrayOES(0);

    return;
}