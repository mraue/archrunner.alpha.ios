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
#include <math.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWController.h"

#include "IWGBufferSubData.h"
#include "IWGMultiBuffer.h"

#include "IWTimer.h"

#include "IWGameData.h"

#include "IWUserInterface.h"

#include "IWCubeCounter.h"
#include "IWFuel.h"

void IWGameSetup(void)
{
//    gdPlayerData = IWPlayerDataMake(IWVector3Make(1.4, 0.8, 1.4),
//                                    IWVector3Normalize(IWVector3Make(-1.0, 0.0, -1.0)),
//                                    IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0)));
    gdPlayerDataStart = IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.5),
                                               IWVector3Normalize(IWVector3Make(0.0, 0.0, 1.0)),
                                               IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0)));
    // DEBUG
    //gdPlayerDataStart.speed = 0.0;
    // END DEBUG
    gdPlayerData = gdPlayerDataStart;
    gdTotalRunTime = 0.0;
    gdRandomRemoveCubeTimer = IWTimerDataMake(0.0, 1.0, false);
    // DEBUG
    //gdRandomRemoveCubeTimer.duration = 0.5 ;
    // END DEBUG
    gdZMax = 0.0;
    gdGameIsPaused = false;
    return;
}

void IWGameRemoveCubeFromBuffer(IWCubeData *cube, IWGMultiBufferData *buffer)
{
    gdBufferToCubeMapNEntries -= 1;
    cube->isInteractive = false;
    
    unsigned int i, currentBufferID, newCubeID;

    currentBufferID = cube->triangleBufferData.bufferIDGPU;
    i = gdBufferToCubeMap[currentBufferID];
    newCubeID = gdBufferToCubeMap[gdBufferToCubeMapNEntries];
    
    if (newCubeID != i) {
        gdBufferToCubeMap[currentBufferID] = newCubeID;
        gdCubeData[newCubeID].triangleBufferData.bufferIDGPU = currentBufferID;
        
        IWGMultiBufferSubData(&gdTriangleDoubleBuffer,
                               cube->triangleBufferData.size * currentBufferID * sizeof(GLfloat),
                               cube->triangleBufferData.size * sizeof(GLfloat),
                               gdCubeData[newCubeID].triangleBufferData.startCPU,
                               false);
    }
    gdTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentDataUpdateBuffer] -= cube->triangleBufferData.size / cube->triangleBufferData.stride;
//    for (unsigned int k=0; k < IWGMULTIBUFFER_MAX; k++) {
//        gdTriangleDoubleBuffer.nVertices[k] -= cube->triangleBufferData.size / cube->triangleBufferData.stride;
//    }
    return;
}

void IWGameUpdate(float timeSinceLastUpdate)
{
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton, gdIsTouched, gdTouchPoint)) {
        if (gdGameIsPaused) {
            gdGameIsPaused = false;
            gdClearColor = IWVector4Make(0.55, 0.55, 0.55, 1.0);
            gdMasterShaderID = 2;
        } else {
            gdGameIsPaused = true;
            gdPauseTime = 0.0;
            gdClearColor = IWVector4Make(0.9, 0.9, 0.9, 1.0);
            gdMasterShaderID = 3;
        }
    }
    
    if (!gdRectangleButton.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton.colorTransition, timeSinceLastUpdate);
        gdRectangleButton.color = gdRectangleButton.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);
        IWGMultiBufferSubDataForBufferObject(&gdUITriangleDoubleBuffer, &gdRectangleButton.triangleBuffer, true);
    }
    
    if (gdGameIsPaused) {
        return;
    }
    
    gdTotalRunTime += timeSinceLastUpdate;
    
    gdZMax = MAX(gdZMax, gdPlayerData.position.z);
    
    // Switch draw buffer
    IWGMultiBufferSwitchBuffer(&gdTriangleDoubleBuffer);

    // Update overdrive
    IWPlayerUpdateOverdrive(&gdPlayerData, timeSinceLastUpdate);
    
    // Update fuel
    if (gdTotalRunTime > 5.0 && !gdPlayerData.overdrive) {
        IWFuelRemoveFuel(&gdFuel, 0.05 * timeSinceLastUpdate);
    }

    // Auto remove standard cubes
    if (!gdPlayerData.overdrive
        && gdStandardCubeIndexList.nEntries > 0
        && IWTimerUpdate(&gdRandomRemoveCubeTimer, timeSinceLastUpdate)) {

        IWTimerResetAndStart(&gdRandomRemoveCubeTimer);

        // Remove cube
        unsigned int i = IWIndexListRemoveRandom(&gdStandardCubeIndexList);
        IWGameRemoveCubeFromBuffer(&gdCubeData[i], &gdTriangleDoubleBuffer);
        
        gdCubeCounter.spawned--;
    }
    
    // Collision detection
    for (int i = 0; i < gdNCubes; i++) {
        if (gdCubeData[i].isInteractive) { //gdCubeData[i].isVisible
            //&& gdCubeData[i].isInteractive
            //&& gdCubeData[i].positionTransition.transitionHasFinished
            //&& gdCubeData[i].collisionRadius > 0.0) {
            if (IWVector3DistanceSquared(gdPlayerData.position, gdCubeData[i].centerPosition)
                < gdCubeData[i].collisionRadius * gdCubeData[i].collisionRadius) {
                // Collision detected
                IWFuelAddFuel(&gdFuel, gdFuel.currentMaxLevel);

                gdCubeData[i].isInteractive = false;
                
                if (gdCubeData[i].type == IWCUBE_TYPE_STANDARD) {

                    // Remove from standard cube list
                    IWIndexListReplaceWithLast(&gdStandardCubeIndexList, gdStandardCubeIndexList.reverseMap[i]);

                    // Move cube
                    gdCubeData[i].type = IWCUBE_TYPE_TRANSITION;

                    IWVector3 newPosition = gdSecondaryPosition[gdSecondaryPositionCounter];
                    float transitionTime = IWVector3Distance(newPosition, gdCubeData[i].centerPosition) / 0.25;
                    gdSecondaryPositionCounter++;
                    gdCubeData[i].positionTransition = IWVector3TransitionMake(gdCubeData[i].centerPosition,
                                                                               newPosition,
                                                                               gdCubeData[i].centerPosition,
                                                                               transitionTime, 0.0, false, false);
                    //IWGPrimitiveBufferDataUpdateColor(&gdCubeData[i].triangleBufferData, IWUI_COLOR_DARK_GOLD(1.0));
                    //gdCubeData[i].color = IWUI_COLOR_DARK_GOLD(1.0);
                    IWGPrimitiveBufferDataUpdateColor(&gdCubeData[i].triangleBufferData, IWUI_COLOR_RED(1.0));
                    gdCubeData[i].color = IWUI_COLOR_DARK_RED(1.0);
                    //gdCubeData[i].color = IWUI_COLOR_DARK_PURPLE(1.0);
                    //gdClearColorTransition.startColor = IWUI_COLOR_DARK_BLUE(1.0);

                    gdCubeCounter.bridge++;
                    gdCubeCounter.spawned--;

                } else if (gdCubeData[i].type == IWCUBE_TYPE_OVERDRIVE) {

                    // Remove cube
                    IWGameRemoveCubeFromBuffer(&gdCubeData[i], &gdTriangleDoubleBuffer);
                    
                    // Activate overdrive
                    IWPlayerActivatOverdrive(&gdPlayerData);
                    IWPlayerUpdateOverdrive(&gdPlayerData, 0.0);
                    
                    IWFuelUpdateColor(&gdFuel, IWUI_COLOR_GOLD(0.4), IWFUEL_COLOR_CURRENT, false);
                    gdOverdriveColorTransition.currentColor = IWUI_COLOR_GOLD(0.4);
                    gdOverdriveColorTransition.currentTransitionTime = 0.0;
                    gdOverdriveColorTransition.transitionHasFinished = false;
                    
                    //gdClearColorTransition.startColor = IWUI_COLOR_DARK_GOLD(1.0);
                    
                    gdCubeCounter.bridge--;
                    gdCubeCounter.pool++;
                    
                    gdClearColorTransition.currentTransitionTime = 0.0;
                    gdClearColorTransition.transitionHasFinished = false;
                }
            }
        } else if (!gdCubeData[i].positionTransition.transitionHasFinished) {
            
            IWVector3TransitionUpdate(&gdCubeData[i].positionTransition, timeSinceLastUpdate);

            if (gdCubeData[i].positionTransition.transitionHasFinished) {
                gdCubeData[i].centerPosition = gdCubeData[i].positionTransition.endVector;
                if (gdCubeData[i].type == IWCUBE_TYPE_TRANSITION) {
                    // Cube has arrived at bridge position
                    //IWGPrimitiveBufferDataUpdateColor(&gdCubeData[i].triangleBufferData, IWUI_COLOR_GOLD(1.0));
                    gdCubeData[i].color = IWUI_COLOR_RED(1.0);
                    gdCubeData[i].isInteractive = true;
                    gdCubeData[i].type = IWCUBE_TYPE_OVERDRIVE;
                } else {
                    //gdCubeData[i].color = IWUI_COLOR_BLUE(1.0);
                    gdCubeData[i].isInteractive = true;
                    gdCubeData[i].type = IWCUBE_TYPE_STANDARD;
                }
            } else {
                // Continue moving cube
                gdCubeData[i].centerPosition = gdCubeData[i].positionTransition.currentVector;  
            }
            IWCubeToTriangles(&gdCubeData[i]);
            IWGMultiBufferSubData(&gdTriangleDoubleBuffer,
                                   gdCubeData[i].triangleBufferData.size * gdCubeData[i].triangleBufferData.bufferIDGPU * sizeof(GLfloat),
                                   gdCubeData[i].triangleBufferData.size * sizeof(GLfloat),
                                   gdCubeData[i].triangleBufferData.startCPU,
                                   false);
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Switch UI VBO object
    IWGMultiBufferSwitchBuffer(&gdUITriangleDoubleBuffer);    
    
    // Update background color transition
    if (!gdClearColorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdClearColorTransition, timeSinceLastUpdate);
    }
    
    // Update fuel status bar
    if (gdFuel.currentLevel == 0.0) {
        // DEBUG
        //gdPlayerData = gdPlayerDataStart;
        //gdFuel.currentLevel = gdFuel.currentMaxLevel;
    } else if (gdPlayerData.overdrive) {
        if (IWColorTransitionUpdate(&gdOverdriveColorTransition, timeSinceLastUpdate)) {
            gdOverdriveColorTransition.endColor = gdOverdriveColorTransition.startColor;
            gdOverdriveColorTransition.startColor = gdOverdriveColorTransition.currentColor;
            gdOverdriveColorTransition.currentTransitionTime = 0.0;
            gdOverdriveColorTransition.transitionHasFinished = false;
        }
        IWFuelUpdateColor(&gdFuel, gdOverdriveColorTransition.currentColor, IWFUEL_COLOR_CURRENT, false);
    } else if (gdFuel.currentLevel / gdFuel.currentMaxLevel < 0.333) {
        if (!gdFuel.isWarning) {
            IWFuelUpdateColor(&gdFuel, gdFuel.warningColor, IWFUEL_COLOR_CURRENT, false);
            gdFuel.isWarning = true;
        }
    } else if (!gdPlayerData.overdrive) {
        IWFuelUpdateColor(&gdFuel, gdFuel.currentColor, IWFUEL_COLOR_CURRENT, false);
        gdFuel.isWarning = false;
    }
    
    // Fuel vertex update
    IWFuelToTriangleBuffer(&gdFuel, gdFuel.stateBar.triangleBufferData.bufferStartCPU);
    
    IWGMultiBufferSubData(&gdUITriangleDoubleBuffer,
                           (gdFuel.stateBar.triangleBufferData.bufferStartCPU - gdRectangleButton.triangleBuffer.bufferStartCPU)  * sizeof(GLfloat),
                           gdFuel.stateBar.triangleBufferData.size * sizeof(GLfloat),
                           gdFuel.stateBar.triangleBufferData.bufferStartCPU,
                           false);
    
//    // Update cube counter bar
//    IWCubeCounterUpdateStateBar(&gdCubeCounter);
//    IWUIStateBarToTriangles(&gdCubeCounter.stateBar);
//    IWGMultiBufferSubDataForBufferObject(&gdUITriangleDoubleBuffer, &gdCubeCounter.stateBar.triangleBufferData, false);
    
    // Check button interaction

    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton2, gdIsTouched, gdTouchPoint)) {
        gdPlayerData = gdPlayerDataStart;
        gdFuel.currentLevel = gdFuel.currentMaxLevel;
    }
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton3, gdIsTouched, gdTouchPoint)) {
        gdResetControllerPosition = true;
    }
    
    if (!gdRectangleButton2.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton2.colorTransition, timeSinceLastUpdate);
        gdRectangleButton2.color = gdRectangleButton2.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton2);
        IWGMultiBufferSubDataForBufferObject(&gdUITriangleDoubleBuffer, &gdRectangleButton2.triangleBuffer, false);
    }
    if (!gdRectangleButton3.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton3.colorTransition, timeSinceLastUpdate);
        gdRectangleButton3.color = gdRectangleButton3.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton3);
        IWGMultiBufferSubDataForBufferObject(&gdUITriangleDoubleBuffer, &gdRectangleButton3.triangleBuffer, false);
    }
    glBindVertexArrayOES(0);

    return;
}