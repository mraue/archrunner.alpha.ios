//
//  IWGame.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWGame.h"

#include "IWMath.h"
#include "IWController.h"

#include "IWGameData.h"

void IWGameSetup(void)
{
    gdPlayerData = IWPlayerDataMake(IWVector3Make(1.4, 0.8, 1.4),
                                    IWVector3Normalize(IWVector3Make(-1.0, 0.0, -1.0)),
                                    IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0)));
    return;
}

void IWGameUpdate(float timeSinceLastUpdate)
{
    IWFuelRemoveFuel(&gdFuel, 0.05 * timeSinceLastUpdate);
    
    // BEGIN TESTING
    glBindVertexArrayOES(gdVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, gdVertexBuffer);
    
    for (int i = 0; i < gdNCubes; i++) {
        if (gdCubeData[i].isVisible && gdCubeData[i].collisionRadius > 0.0) {
            if (IWVector3DistanceSquared(gdPlayerData.position, gdCubeData[i].centerPosition)
                < gdCubeData[i].collisionRadius * gdCubeData[i].collisionRadius) {
                // Collision detected
                // Hide cube
                gdCubeData[i].isVisible = false;
                if (gdBufferToCubeMapNEntries > 0) {
                    gdBufferToCubeMapNEntries -= 1;
                    unsigned currentBufferID, newCubeID;
                    //currentBufferID = gdCubeToBufferMap[i];
                    currentBufferID = gdCubeData[i].triangleBufferData.bufferIDGPU;
                    newCubeID = gdBufferToCubeMap[gdBufferToCubeMapNEntries];
                    if (newCubeID != i) {
                        gdBufferToCubeMap[currentBufferID] = newCubeID;
                        //gdCubeToBufferMap[newCubeID] = currentBufferID;
                        gdCubeData[newCubeID].triangleBufferData.bufferIDGPU = currentBufferID;
                        glBufferSubData(GL_ARRAY_BUFFER,
                                        gdCubeData[i].triangleBufferData.size * currentBufferID * sizeof(GLfloat),
                                        gdCubeData[i].triangleBufferData.size * sizeof(GLfloat),
                                        gdCubeData[newCubeID].triangleBufferData.startCPU);
                    }
                    printf("BOING %u %u %u\n", i, currentBufferID, newCubeID);
                    gdFuel.currentLevel = gdFuel.currentMaxLevel;
                }
                //glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
                gdN_VERT -= gdCubeData[i].triangleBufferData.size / gdCubeData[i].triangleBufferData.stride;
                gdClearColorTransition.currentTransitionTime = 0.0;
                gdClearColorTransition.transitionHasFinished = false;
            }
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // END TESTING
    
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

    glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER,
                    (gdFuel.stateBar.uiElementData.triangleBufferStart - gdRectangleButton.triangleBufferStart)  * sizeof(GLfloat),
                    gdFuel.stateBar.uiElementData.triangleBufferSize * sizeof(GLfloat), gdFuel.stateBar.uiElementData.triangleBufferStart);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
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

        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, gdRectangleButton.triangleBufferSize * sizeof(GLfloat), gdRectangleButton.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArrayOES(0);
    }
    if (!gdRectangleButton2.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton2.colorTransition, timeSinceLastUpdate);
        gdRectangleButton2.color = gdRectangleButton2.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton2);
        
        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, (gdRectangleButton2.triangleBufferStart - gdRectangleButton.triangleBufferStart) * sizeof(GLfloat),
                        gdRectangleButton2.triangleBufferSize * sizeof(GLfloat), gdRectangleButton2.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArrayOES(0);
    }
    if (!gdRectangleButton3.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton3.colorTransition, timeSinceLastUpdate);
        gdRectangleButton3.color = gdRectangleButton3.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton3);
        
        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, (gdRectangleButton3.triangleBufferStart - gdRectangleButton.triangleBufferStart)  * sizeof(GLfloat),
                        gdRectangleButton3.triangleBufferSize * sizeof(GLfloat), gdRectangleButton3.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArrayOES(0);
    }
    glBindVertexArrayOES(0);
    return;
}