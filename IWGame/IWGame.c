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

#include "IWGRenderer.h"

void IWGameSetup(void)
{
    //
    gdCurrentGameStatus = IWGAME_STATUS_START_MENU;
    //
    gdRandomRemoveCubeTimer = IWTimerDataMake(0.0, 1.0, false);
    gdStateSwitchTimer = IWTimerDataMake(0.0, 0.5, false);
    //gdGameIsPaused = false;
    gdNCubesPerAxis = 5;// [5]
    //
    gdCubeTriangleBufferStartCPU = NULL;
    gdSkyTriangleBufferStartCPU = NULL;
    gdInGameTextTriangleBufferStartCPU = NULL;
    gdInGameUITriangleBufferStartCPU = NULL;
    gdInGameUILineBufferStartCPU = NULL;
    gdCubeData = NULL;
    //
    gdControllerDataAccelerometer = IWControllerDataMakeDefault();
    //
    IWGameReset();
    //
    return;
}

void IWGameReset(void)
{
    IWVector3 playerPosition = IWVector3Make(-0.4, 0.4, -1.2);
    IWVector3 playerDirection = IWVector3Normalize(IWVector3Make(0.25, -0.25, 1.0));
    IWVector3 normalDirectionVec = IWVector3CrossProduct(IWVector3Make(0.0, 0.0, 1.0), playerDirection);
    float angle = IWVector3DotProduct(IWVector3Make(0.0, 0.0, 1.0), playerDirection);
    IWMatrix4 rotationMatrix = IWMatrix4MakeRotation(acosf(angle), normalDirectionVec.x, normalDirectionVec.y , normalDirectionVec.z);
    IWVector3 playerUp = IWMatrix4MultiplyVector3(rotationMatrix, IWVector3Make(0.0, 1.0, 0.0));
    
    gdPlayerData = gdPlayerDataStart = IWPlayerDataMakeSimple(playerPosition,
                                                              playerDirection,
                                                              playerUp);
    gdTotalRunTime = 0.0;
    gdZMax = 0.0;
    gdScoreCounter = IWScoreCounterMakeEmpty();
    gdFuel = IWFuelMakeDefaultStart();
    //IWFuelRemoveFuel(&gdFuel, 0.95);
}

void IWGameMainHandler(float timeSinceLastUpdate, float aspectRatio)
{
    if (gdCurrentGameStatus == IWGAME_STATUS_RUNNING
        || gdCurrentGameStatus == IWGAME_STATUS_PAUSED
        || gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER) {
        IWGameUpdate(timeSinceLastUpdate, aspectRatio);
    } else if (gdCurrentGameStatus == IWGAME_STATUS_START_MENU) {
        IWGameStartMenuHandler(timeSinceLastUpdate, aspectRatio);
    }
    return;
}

void IWGameStartMenuHandler(float timeSinceLastUpdate, float aspectRatio)
{
    IWRectangle startButton = IWRectangleMake(0.5, 0.2, 1.0, 0.8);
    if (IWTimerUpdate(&gdStateSwitchTimer, timeSinceLastUpdate)
        && gdIsTouched
        && IWPointInRectangle(gdTouchPoint, startButton)) {
        IWGRendererTearDownStartMenuAssets();
        IWGRendererSetupGameAssets();
        gdCurrentGameStatus = IWGAME_STATUS_RUNNING;
        return;
    }
    
    IWGMultiBufferSwitchBuffer(&gdUITriangleDoubleBuffer);

    IWGMultiBufferSwitchBuffer(&gdTextTriangleDoubleBuffer);
    
    if (IWVector4TransitionUpdate(&gdStartTextFieldColorTransition, timeSinceLastUpdate)) {
        IWVector4TransitionReverseAndStart(&gdStartTextFieldColorTransition);
    }
    gdStartTextField.color = gdStartTextFieldColorTransition.currentVector;
    IWGTextFieldSetText(&gdStartTextField, gdStartTextField.text);
    IWGMultiBufferSubData(&gdTextTriangleDoubleBuffer,
                          (gdTitleTextField.triangleBufferData.size + gdVersionTextField.triangleBufferData.size) * sizeof(GLfloat),
                          gdStartTextField.triangleBufferData.size * sizeof(GLfloat),
                          gdStartTextField.triangleBufferData.startCPU,
                          false);
    
    // Setup view matrices
    IWMatrix4 projectionMatrix = IWMatrix4MakePerspective(65.0f * IW_DEG_TO_RAD, aspectRatio, 0.01f, 100.0f);
    
    // Compute the model view matrix for the object rendered with ES2
    // REFACTOR: does not change, could only be calculated and intialized to uniforms once
    IWMatrix4 modelMatrix = IWMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);
    
    IWMatrix4 viewMatrix = IWMatrix4MakeLookAt(gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z,
                                               gdPlayerData.position.x + gdPlayerData.direction.x,
                                               gdPlayerData.position.y + gdPlayerData.direction.y,
                                               gdPlayerData.position.z + gdPlayerData.direction.z,
                                               gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    
    gdNormalMatrix = IWMatrix4GetMatrix3(modelMatrix);//GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelMatrix), NULL);
    gdModelMatrix = modelMatrix;
    gdProjectionMatrix = projectionMatrix;
    gdViewMatrix = viewMatrix;
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
    
    return;
}

void IWGameUpdate(float timeSinceLastUpdate,
                  float aspectRatio)
{
    // Check if we are game over
    if (gdFuel.currentLevel == 0.0
        || gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER) {
        if (gdCurrentGameStatus == IWGAME_STATUS_RUNNING) {
            printf("GAME OVER\n");
            IWScoreCounterPrintScore(&gdScoreCounter);
            gdCurrentGameStatus = IWGAME_STATUS_PAUSED;
            for (unsigned int  k =0; k < IWGMULTIBUFFER_MAX; k++) {
                gdTextTriangleDoubleBuffer.nVertices[k] =
                    (gdScoreTextField.triangleBufferData.size
                     + gdGameOverTextField.triangleBufferData.size
                     + gdGameOverMenuTextField.triangleBufferData.size) / gdScoreTextField.triangleBufferData.stride;
            }
            gdMasterShaderID = 3;
            gdSkyShaderID = 3;
            gdClearColor = IWVector4Make(0.9, 0.9, 0.9, 1.0);
            gdCurrentGameStatus = IWGAME_STATUS_GAME_OVER;
        }
        IWRectangle toStartMenuRect = IWRectangleMake(0.0, 0.2, 0.5, 0.35);
        IWRectangle retryRect = IWRectangleMake(0.0, 0.35, 0.5, 0.5);
        if (gdIsTouched
            && IWPointInRectangle(gdTouchPoint, toStartMenuRect)) {
            IWGRendererTearDownGameAssets();
            IWGRendererSetupStartMenuAssets();
            gdCurrentGameStatus = IWGAME_STATUS_START_MENU;
            gdIsTouched = false;
            gdStateSwitchTimer = IWTimerDataMake(0.0, 0.5, false);
            return;
        } else if (gdIsTouched
                    && IWPointInRectangle(gdTouchPoint, retryRect)) {
            IWGRendererTearDownGameAssets();
            IWGRendererSetupGameAssets();
            gdCurrentGameStatus = IWGAME_STATUS_RUNNING;
            gdIsTouched = false;
            return;
        }
        if (IWVector4TransitionUpdate(&gdStartTextFieldColorTransition, timeSinceLastUpdate)) {
            IWVector4TransitionReverseAndStart(&gdStartTextFieldColorTransition);
        }
        gdGameOverMenuTextField.color = gdStartTextFieldColorTransition.currentVector;
        IWGTextFieldSetText(&gdGameOverMenuTextField, gdGameOverMenuTextField.text);
        IWGMultiBufferSubData(&gdTextTriangleDoubleBuffer,
                              (gdScoreTextField.triangleBufferData.size
                               + gdGameOverTextField.triangleBufferData.size) * sizeof(GLfloat),
                              gdGameOverMenuTextField.triangleBufferData.size * sizeof(GLfloat),
                              gdGameOverMenuTextField.triangleBufferData.startCPU,
                              false);
        return;
    }
        
    // Check if pause button has been pressed
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton, gdIsTouched, gdTouchPoint)) {
        if (gdCurrentGameStatus == IWGAME_STATUS_PAUSED) {
            //gdGameIsPaused = false;
            gdCurrentGameStatus = IWGAME_STATUS_RUNNING;
            gdClearColor = IWVector4Make(0.6, 0.6, 0.6, 1.0);
            gdMasterShaderID = 2;
            gdSkyShaderID = 4;
        } else {
            //gdGameIsPaused = true;
            gdCurrentGameStatus = IWGAME_STATUS_PAUSED;
            gdPauseTime = 0.0;

        }
    }
    
    if (gdCurrentGameStatus == IWGAME_STATUS_PAUSED) {
        //gdClearColor = IWVector4Make(0.9, 0.9, 0.9, 1.0);
        //gdSkyShaderID = gdMasterShaderID = 3;
    }
    
    if (!gdRectangleButton.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton.colorTransition, timeSinceLastUpdate);
        gdRectangleButton.color = gdRectangleButton.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);
        IWGMultiBufferSubDataForBufferObject(&gdUITriangleDoubleBuffer, &gdRectangleButton.triangleBuffer, true);
    }
    
    if (gdCurrentGameStatus == IWGAME_STATUS_PAUSED) {
        return;
    }
    
    // Update player position
    float speed = gdCurrentGameStatus == IWGAME_STATUS_PAUSED ? 0.0 : gdPlayerData.speed;
    gdPlayerData.position = IWVector3Add(gdPlayerData.position,
                                         IWVector3MultiplyScalar(IWVector3Normalize(gdPlayerData.direction),
                                                                 timeSinceLastUpdate * speed));
    
    float rotationSpeedMax = 100.0 / 180.0 * M_PI * timeSinceLastUpdate;
    
    // Update direction
    IWVector3 dirGLV = IWVector3Make(gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z);
    IWVector3 upGLV = IWVector3Make(gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    
    IWMatrix4 yRotationUpdateMatrix =
        IWMatrix4MakeRotation(gdControllerDataAccelerometer.rotationSpeed.y * rotationSpeedMax,
                              upGLV.x, upGLV.y, upGLV.z);
    
    IWVector3 normGLV = IWVector3CrossProduct(dirGLV, upGLV);
    
    IWMatrix4 xRotationUpdateMatrix =
        IWMatrix4MakeRotation(gdControllerDataAccelerometer.rotationSpeed.x * rotationSpeedMax,
                              normGLV.x, normGLV.y, normGLV.z);
    
    IWMatrix4 rotationUpdateMatrix = IWMatrix4Multiply(xRotationUpdateMatrix, yRotationUpdateMatrix);
    
    //    if (motionManager.isDeviceMotionAvailable) {
    //        GLKMatrix4 zRotationUpdateMatrix = GLKMatrix4MakeRotation(rotationSpeedZ * rotationSpeedMax,
    //                                                                  dirGLV.x, dirGLV.y, dirGLV.z);
    //        rotationUpdateMatrix = GLKMatrix4Multiply(rotationUpdateMatrix, zRotationUpdateMatrix);
    //    }
    
    gdPlayerData.direction = IWMatrix4MultiplyVector3(rotationUpdateMatrix, dirGLV);
    gdPlayerData.up = IWMatrix4MultiplyVector3(rotationUpdateMatrix, upGLV);
    
    //
    //
    //
    
    gdTotalRunTime += timeSinceLastUpdate;
    
    // Udpate score and score display
    IWGMultiBufferSwitchBuffer(&gdTextTriangleDoubleBuffer);
    
    gdScoreCounter.runningTimeTotal += timeSinceLastUpdate;

    gdZMax = IW_MAX(gdZMax, gdPlayerData.position.z);
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
        && gdPlayerData.position.z + 0.5 >= gdSecondaryPosition[gdSecondaryPositionCounter - 1].z) {

        int i;

        IWVector3 newCenter = gdSecondaryPosition[gdSecondaryPositionCounter];
        newCenter.z += 0.6;
        
        free(gdSecondaryPosition);
        gdSecondaryPosition = IWCubeMakeCubeCurve(gdNCubes, newCenter, IWGEOMETRY_AXIS_Z);
        gdSecondaryPositionCounter = 0;
        
        unsigned int nCubesX, nCubesY, nCubesZ;
        nCubesZ = nCubesY = nCubesX = (unsigned int)pow(gdPoolCubeIndexList.nEntries, 1. / 3.);
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
    if (gdPlayerData.overdrive) {
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
        IWGRendererTearDownGameAssets();
        IWGRendererSetupGameAssets();
        //gdGameIsPaused = false;
        gdCurrentGameStatus = IWGAME_STATUS_RUNNING;
    }
    
    if (!gdRectangleButton2.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton2.colorTransition, timeSinceLastUpdate);
        gdRectangleButton2.color = gdRectangleButton2.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton2);
        IWGMultiBufferSubDataForBufferObject(&gdUITriangleDoubleBuffer, &gdRectangleButton2.triangleBuffer, false);
    }
    glBindVertexArrayOES(0);
    
    // Setup view matrices
    IWMatrix4 projectionMatrix = IWMatrix4MakePerspective(65.0f * IW_DEG_TO_RAD, aspectRatio, 0.01f, 100.0f);
    
    // Compute the model view matrix for the object rendered with ES2
    // REFACTOR: does not change, could only be calculated and intialized to uniforms once
    IWMatrix4 modelMatrix = IWMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);
    
    IWMatrix4 viewMatrix = IWMatrix4MakeLookAt(gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z,
                                               gdPlayerData.position.x + gdPlayerData.direction.x,
                                               gdPlayerData.position.y + gdPlayerData.direction.y,
                                               gdPlayerData.position.z + gdPlayerData.direction.z,
                                               gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    
    gdNormalMatrix = IWMatrix4GetMatrix3(modelMatrix);//GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelMatrix), NULL);
    gdModelMatrix = modelMatrix;
    gdProjectionMatrix = projectionMatrix;
    gdViewMatrix = viewMatrix;

    return;
}