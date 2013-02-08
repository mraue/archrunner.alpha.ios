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

#include "IWFuel.h"

#include "IWScoreCounter.h"

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
    // DEBUG
    gdSpawnCubes = true;
    //
    gdScoreCounter = IWScoreCounterMakeEmpty();
    //
    return;
}

void IWGameRemoveCubeFromBuffer(IWCubeData *cube, IWGMultiBufferData *buffer)
{
    int bufferId = IWIndexListGetIndexForObjectId(&gdGPUBufferPositionIndexList, cube->id);
    if (bufferId < 0) {
        printf("ERROR: Could not find cube with id %u in GPU buffer list\n", cube->id);
        return;
    }

    int newBufferId = IWIndexListReplaceWithLast(&gdGPUBufferPositionIndexList, bufferId);

    cube->isInteractive = false;
    cube->type = IWCUBE_TYPE_POOL;

    //currentBufferID = cube->triangleBufferData.bufferIDGPU;
    if (bufferId != gdGPUBufferPositionIndexList.nEntries) {
        int newCubeID = IWIndexListGetObjectIdForIndex(&gdGPUBufferPositionIndexList, bufferId);
        if (newCubeID < 0) {
            printf("ERROR could not get object id from gdGPUBufferPositionIndexList for buffer %i", bufferId);
            return;
        }
        
        if (newBufferId != bufferId) {
            gdCubeData[newCubeID].triangleBufferData.bufferIDGPU = bufferId;
            
            IWGMultiBufferSubData(&gdTriangleDoubleBuffer,
                                   cube->triangleBufferData.size * bufferId * sizeof(GLfloat),
                                   cube->triangleBufferData.size * sizeof(GLfloat),
                                   gdCubeData[newCubeID].triangleBufferData.startCPU,
                                   false);
        }
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
            gdClearColor = IWVector4Make(0.6, 0.6, 0.6, 1.0);
            gdMasterShaderID = 2;
            gdSkyShaderID = 4;
        } else {
            gdGameIsPaused = true;
            gdPauseTime = 0.0;

        }
    }
    
    if (gdGameIsPaused) {
        //gdClearColor = IWVector4Make(0.9, 0.9, 0.9, 1.0);
        //gdSkyShaderID = gdMasterShaderID = 3;
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
    
    // Udpate score and score display
    IWGMultiBufferSwitchBuffer(&gdTextTriangleDoubleBuffer);
    
    gdScoreCounter.runningTimeTotal += timeSinceLastUpdate;

    gdZMax = MAX(gdZMax, gdPlayerData.position.z);
    gdScoreCounter.zMax = gdZMax;
    
    unsigned int oldScore = gdScoreCounter.scoreInt;
    IWScoreCounterUpdateScore(&gdScoreCounter);
    
    if (gdScoreCounter.scoreInt != oldScore) {
        char s[10];
        sprintf(s, "%u", gdScoreCounter.scoreInt);
        IWGTextFieldSetText(&gdScoreTextField, s);
        IWGMultiBufferSubData(&gdTextTriangleDoubleBuffer,
                              0,
                              gdScoreTextField.triangleBufferData.size * sizeof(GLfloat),
                              gdScoreTextField.triangleBufferData.startCPU,
                              false);
    }
    
    // Switch main draw buffer
    IWGMultiBufferSwitchBuffer(&gdTriangleDoubleBuffer);

    // Update overdrive
    IWPlayerUpdateOverdrive(&gdPlayerData, timeSinceLastUpdate);
    
    // Update fuel
    if (gdTotalRunTime > 5.0 && !gdPlayerData.overdrive) {
        IWFuelRemoveFuel(&gdFuel, 0.05 / 0.6 * timeSinceLastUpdate);
    }

    // Spawn pooled cubes
    if (gdPoolCubeIndexList.nEntries > 10
        && gdPlayerData.position.z + 0.5 >= gdSecondaryPosition[gdSecondaryPositionCounter - 1].z
        && gdSpawnCubes) {

        int i;

        IWVector3 newCenter = gdSecondaryPosition[gdSecondaryPositionCounter];
        newCenter.z += 0.6;
        
        free(gdSecondaryPosition);
        gdSecondaryPosition = IWCubeMakeCubeCurve(gdNCubes, newCenter, IWGEOMETRY_AXIS_Z);
        gdSecondaryPositionCounter = 0;
        
        unsigned int nCubesX, nCubesY, nCubesZ;
        nCubesZ = nCubesY = nCubesX = (unsigned int)pow(gdPoolCubeIndexList.nEntries, 1. / 3.);
        //nCubesZ = (unsigned int)gdPoolCubeIndexList.nEntries / nCubesX / nCubesY;
        nCubesX++; nCubesY++; nCubesZ++;
        IWVector3 *newPositions = IWCubeMakeCubePositions(nCubesX, nCubesY, nCubesZ,
                                                          .04, .12,
                                                          newCenter,
                                                          1, 0.05);
        
        for (int j = 0; j < gdPoolCubeIndexList.nEntries; j++) {
            i = gdPoolCubeIndexList.map[j];
            // Move cube
            gdCubeData[i].type = IWCUBE_TYPE_SPAWNING;
            gdCubeData[i].color = IWVector4Make(0.5, 0.5, 0.5, 1.0);
            gdCubeData[i].isInteractive = false;
            
            IWVector3 randomOffset = IWVector3Make(IW_RAND_SIGN * IW_RAND_UNIFORM(2.0, 4.0),
                                                   IW_RAND_SIGN * IW_RAND_UNIFORM(2.0, 4.0),
                                                   IW_RAND_SIGN * IW_RAND_UNIFORM(2.0, 4.0));
            IWVector3 spawnPosition = IWVector3Add(newCenter, randomOffset);
//            IWVector3 randomOffset2 = IWVector3Make(IW_RAND_SIGN * IW_RAND_UNIFORM(0.0, 0.3),
//                                                   IW_RAND_SIGN * IW_RAND_UNIFORM(0.0, 0.3),
//                                                   IW_RAND_SIGN * IW_RAND_UNIFORM(0.0, 0.3));
            float transitionTime = 2.0;
            
            gdCubeData[i].positionTransition = IWVector3TransitionMake(spawnPosition,
                                                                       newPositions[j],//IWVector3Add(newCenter, randomOffset2),
                                                                       spawnPosition,
                                                                       transitionTime, 0.0, false, false);
            gdCubeData[i].centerPosition = spawnPosition;
            IWIndexListAppendObjectId(&gdStandardCubeIndexList, i);
            gdCubeData[i].triangleBufferData.bufferIDGPU = IWIndexListAppendObjectId(&gdGPUBufferPositionIndexList, i);
            gdTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentDataUpdateBuffer] += gdCubeData[i].triangleBufferData.size / gdCubeData[i].triangleBufferData.stride;
        }
        
        free(newPositions);
        
        gdPoolCubeIndexList.nEntries = 0;
        //gdSpawnCubes = false;
    }
    
    // Auto remove standard cubes
    if (!gdPlayerData.overdrive
        && gdStandardCubeIndexList.nEntries > 0
        && IWTimerUpdate(&gdRandomRemoveCubeTimer, timeSinceLastUpdate)) {

        IWTimerResetAndStart(&gdRandomRemoveCubeTimer);

        // Remove cube
        unsigned int i = IWIndexListRemoveRandom(&gdStandardCubeIndexList);
        //printf("DEBUG: Removing cube %u\n", i);
        IWGameRemoveCubeFromBuffer(&gdCubeData[i], &gdTriangleDoubleBuffer);
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
                    IWIndexListReplaceWithLast(&gdStandardCubeIndexList,
                                               IWIndexListGetIndexForObjectId(&gdStandardCubeIndexList, i));

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

                    gdScoreCounter.nGridCubes++;

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
                    
                    gdScoreCounter.nBridgeCubes++;
                    
                    gdClearColorTransition.currentTransitionTime = 0.0;
                    gdClearColorTransition.transitionHasFinished = false;
                    IWIndexListAppendObjectId(&gdPoolCubeIndexList, i);
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
        printf("GAME OVER\n");
        IWScoreCounterPrintScore(&gdScoreCounter);
        gdGameIsPaused = true;
        gdPauseTime = 0.0;
        gdFuel.currentLevel = 1.0;
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
    
    // Check button interaction

    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton2, gdIsTouched, gdTouchPoint)) {
        gdPlayerData = gdPlayerDataStart;
        gdFuel.currentLevel = gdFuel.currentMaxLevel;
    }
    
    if (!gdRectangleButton2.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton2.colorTransition, timeSinceLastUpdate);
        gdRectangleButton2.color = gdRectangleButton2.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton2);
        IWGMultiBufferSubDataForBufferObject(&gdUITriangleDoubleBuffer, &gdRectangleButton2.triangleBuffer, false);
    }
    glBindVertexArrayOES(0);

    return;
}