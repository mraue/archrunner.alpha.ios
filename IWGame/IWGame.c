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
#include "IWGRingBuffer.h"

#include "IWTimer.h"

#include "IWGameData.h"

#include "IWUserInterface.h"

#include "IWFuel.h"

#include "IWScoreCounter.h"

#include "IWGSkyBoxController.h"

#include "IWGRenderer.h"

void IWGameSetup(void)
{
    IW_RAND_RANDOMIZE_TIMER
    //
    gdCurrentGameStatus = IWGAME_STATUS_START_MENU;
    //gdCurrentGameStatus = IWGAME_STATUS_TUTORIAL;
    //
    gdRandomRemoveCubeTimer = IWTimerDataMake(0.0, 1.1, false);
    gdStateSwitchTimer = IWTimerDataMake(0.0, 0.5, false);
    //
    gdCubeTriangleBufferStartCPU = NULL;
    gdInGameTextTriangleBufferStartCPU = NULL;
//    gdInGameUITriangleBufferStartCPU = NULL;
//    gdInGameUILineBufferStartCPU = NULL;
    gdCubeData = NULL;
    //
    gdControllerDataAccelerometer = IWControllerDataMakeDefault();
    //
    gdGrayScaleTransitionDefault = gdGrayScaleTransition = IWVector3TransitionMake(IWVector3Make(1.0, 0.2, 0.0),
                                                                                   IWVector3Make(0.0, 0.2, 0.0),
                                                                                   IWVector3Make(1.0, 0.2, 0.0),
                                                                                   1.0, 0.0, false, false);
    //
    IWGameReset();
    //
    gdScreenShotUIMode = 0;
    gdScreenShotRotation = IWVector3Make(0.0, 0.0, 0.0);
    //
    gdScreenShotSliderX = IWUISliderMake(IWRectangleMake(0.15, 0.0, 0.85, 0.15));
    gdScreenShotSliderY = IWUISliderMake(IWRectangleMake(0.0, 0.15, 0.15, 0.85));
    gdScreenShotSliderZ = IWUISliderMake(IWRectangleMake(0.85, 0.15, 1.0, 0.85));
    gdScreenShotSliderE = IWUISliderMake(IWRectangleMake(0.15, 0.85, 0.85, 1.0));
    //
    return;
}

void IWGameReset(void)
{
    IWVector3 playerPosition = IWVector3Make(-0.4, 0.4, -1.2);
    IWVector3 playerDirection = IWVector3Normalize(IWVector3Make(0.25, -0.25, 1.0));
    IWVector3 playerUp = IWControllerGetYUpForDirection(playerDirection);
    
    gdPlayerData = gdPlayerDataStart = IWPlayerDataMakeSimple(playerPosition,
                                                              playerDirection,
                                                              playerUp);
    
    gdNCubesPerAxis = 5;// [5]
    gdNCubes = gdNCubesPerAxis * gdNCubesPerAxis * gdNCubesPerAxis;
    
    gdTotalRunTime = 0.0;
    gdZMax = 0.0;
    gdScoreCounter = IWScoreCounterMakeEmpty();
    gdCubeStatus = IWCubeStatusMakeEmpty();
    gdCubeStatus.nGridCubes = gdNCubes;
    gdFuel = IWFuelMakeDefaultStart();
    gdGrayScaleTransition = gdGrayScaleTransitionDefault;
    gdPushScoreToLeaderboard = false;
    gdUpdateAchievements = true;
    // DEBUG
    //IWFuelRemoveFuel(&gdFuel, 0.95);
    // END DEBUG
}

void IWGameMainHandler(float timeSinceLastUpdate, float aspectRatio)
{
    if (gdCurrentGameStatus == IWGAME_STATUS_RUNNING
        || gdCurrentGameStatus == IWGAME_STATUS_PAUSED) {
        IWGameUpdate(timeSinceLastUpdate, aspectRatio);
    } else if (gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER) {
        IWGameGameOverHandler(timeSinceLastUpdate, aspectRatio);
    } else if (gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER_MENU) {
        IWGameGameOverMenuHandler(timeSinceLastUpdate, aspectRatio);
    } else if (gdCurrentGameStatus == IWGAME_STATUS_START_MENU) {
        IWGameStartMenuHandler(timeSinceLastUpdate, aspectRatio);
    }  else if (gdCurrentGameStatus == IWGAME_STATUS_SCREENSHOT) {
        IWGameScreenShotHandler(timeSinceLastUpdate, aspectRatio);
    } else if (gdCurrentGameStatus == IWGAME_STATUS_TUTORIAL) {
        IWGameTutorialHandler(timeSinceLastUpdate, aspectRatio);
    }
    return;
}

void IWGameGameOverHandler(float timeSinceLastUpdate, float aspectRatio)
{
    IWGRingBufferSwitchBuffer(&gdTriangleDoubleBuffer);
    //IWGRingBufferSwitchBuffer(&gdUITriangleDoubleBuffer);
    IWGRingBufferSwitchBuffer(&gdTextTriangleDoubleBuffer);
    
    // Nifty trick to get the menu setup, i.e. flush the buffer updates
    // (this should probably be a function in the future)
    IWGRingBufferSwitchBuffer(&gdPauseMenu.multiBuffer);
    
    if (!gdGrayScaleTransition.transitionHasFinished) {
        IWVector3TransitionUpdate(&gdGrayScaleTransition, timeSinceLastUpdate);
        glUseProgram(gdSkyboxShaderProgram.programID);
        glUniform2f(glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale"),
                    gdGrayScaleTransition.currentVector.x,
                    gdGrayScaleTransition.currentVector.y);
        glUseProgram(gdMainShaderProgram.programID);
        glUniform2f(glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale"),
                    gdGrayScaleTransition.currentVector.x,
                    gdGrayScaleTransition.currentVector.y);
    }
    
    IWScorePresenterUpdate(&gdScorePresenterTest, &gdScoreCounter, timeSinceLastUpdate);
    
    if (gdScorePresenterTest.hasFinished && gdIsTouched) {
        gdCurrentGameStatus = IWGAME_STATUS_GAME_OVER_MENU;
        gdIsTouched = false;
    }

    return;
}

void IWGameGameOverMenuHandler(float timeSinceLastUpdate, float aspectRatio)
{
    if (!gdGrayScaleTransition.transitionHasFinished) {
        IWVector3TransitionUpdate(&gdGrayScaleTransition, timeSinceLastUpdate);
        glUseProgram(gdSkyboxShaderProgram.programID);
        glUniform2f(glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale"),
                    gdGrayScaleTransition.currentVector.x,
                    gdGrayScaleTransition.currentVector.y);
        glUseProgram(gdMainShaderProgram.programID);
        glUniform2f(glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale"),
                    gdGrayScaleTransition.currentVector.x,
                    gdGrayScaleTransition.currentVector.y);
    }
    
    if (gdIsTouched) {
        int touchN = IWUIMenuPresenterGetTouch(&gdPauseMenu.presenter, gdTouchPoint);
        gdIsTouched = false;
        if (touchN == 0) {
            IWGRendererTearDownGameAssets();
            IWGRendererSetupStartMenuAssets();
            gdCurrentGameStatus = IWGAME_STATUS_START_MENU;
            gdIsTouched = false;
            gdStateSwitchTimer = IWTimerDataMake(0.0, 0.5, false);
            glUseProgram(gdSkyboxShaderProgram.programID);
            glUniform2f(glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale"), 0.0, 0.4);
            glUseProgram(gdMainShaderProgram.programID);
            glUniform2f(glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale"), 0.0, 0.4);
            return;
        } else if (touchN == 1) {
            IWGRendererTearDownGameAssets();
            IWGRendererSetupGameAssets();
            gdCurrentGameStatus = IWGAME_STATUS_RUNNING;
            gdIsTouched = false;
            glUseProgram(gdSkyboxShaderProgram.programID);
            glUniform2f(glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale"), 0.0, 0.4);
            glUseProgram(gdMainShaderProgram.programID);
            glUniform2f(glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale"), 0.0, 0.4);
            return;
        }
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
        gdIsTouched = false;
        return;
    }
    
//    IWRectangle screenShotButton = IWRectangleMake(0.0, 0.0, 0.1, 0.1);
//    if (gdIsTouched
//        && IWPointInRectangle(gdTouchPoint, screenShotButton)) {
//        //IWGRendererTearDownStartMenuAssets();
//        //IWGRendererSetupGameAssets();
//        gdCurrentGameStatus = IWGAME_STATUS_SCREENSHOT;
//        //gdPlayerDataStart = gdPlayerData = IWPlayerDataMakeSimple(IWVector3Make(0.332, 0.302, 19.290),IWVector3Normalize(IWVector3Make(0.113, 0.246, 0.963)),IWVector3Normalize(IWVector3Make(-0.508, 0.847, -0.157)));
//        //gdPlayerDataStart = gdPlayerData = IWPlayerDataMakeSimple(IWVector3Make(0.332, 0.295, 19.299),IWVector3Normalize(IWVector3Make(0.137, 0.374, 0.917)),IWVector3Normalize(IWVector3Make(-0.517, 0.817, -0.256)));
//        //gdSkyBox.transitionTime = 319.83;
//        //gdPlayerDataStart = gdPlayerData = IWPlayerDataMakeSimple(IWVector3Make(0.110, 0.138, 19.135),IWVector3Normalize(IWVector3Make(0.090, 0.379, 0.921)),IWVector3Normalize(IWVector3Make(-0.522, 0.806, -0.281)));;
//        //gdSkyBox.transitionTime = 336.06;
//        
//        // Icon
//        //gdPlayerDataStart = gdPlayerData = IWPlayerDataMakeSimple(IWVector3Make(0.014, 0.357, 21.207),IWVector3Normalize(IWVector3Make(0.013, 0.339, 0.941)),IWVector3Normalize(IWVector3Make(-0.510, 0.811, -0.286)));
//        //gdSkyBox.transitionTime = 336.06;
//        // Achievement: Cube Collector (Pro)
//        gdPlayerDataStart = gdPlayerData = IWPlayerDataMakeSimple(IWVector3Make(-1.386, 0.000, -1.208),IWVector3Normalize(IWVector3Make(0.793, 0.000, 0.610)),IWVector3Normalize(IWVector3Make(0.000, 1.000, 0.000)));
//        gdSkyBox.transitionTime = 3.61;
//        gdIsTouched = false;
//        return;
//    }
    
    IWGRingBufferSwitchBuffer(&gdTriangleDoubleBuffer);
    IWGRingBufferSwitchBuffer(&gdTextTriangleDoubleBuffer);
    
    if (IWVector4TransitionUpdate(&gdStartTextFieldColorTransition, timeSinceLastUpdate)) {
        IWVector4TransitionReverseAndStart(&gdStartTextFieldColorTransition);
    }
    gdStartTextField.color = gdStartTextFieldColorTransition.currentVector;
    IWGTextFieldSetText(&gdStartTextField, gdStartTextField.text);
    IWGRingBufferSubData(&gdTextTriangleDoubleBuffer,
                          (gdTitleTextField.triangleBufferData.size + gdVersionTextField.triangleBufferData.size) * sizeof(GLfloat),
                          gdStartTextField.triangleBufferData.size * sizeof(GLfloat),
                          gdStartTextField.triangleBufferData.startCPU,
                          false);
    
    IWGSkyBoxControllerUpdate(&gdSkyBoxController, timeSinceLastUpdate, &gdPlayerData, true);
    
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

void IWGameScreenShotHandler(float timeSinceLastUpdate, float aspectRatio)
{
    
//    IWRectangle startButton = IWRectangleMake(0.5, 0.2, 1.0, 0.8);
//    if (IWTimerUpdate(&gdStateSwitchTimer, timeSinceLastUpdate)
//        && gdIsTouched
//        && IWPointInRectangle(gdTouchPoint, startButton)) {
//        IWGRendererTearDownStartMenuAssets();
//        IWGRendererSetupGameAssets();
//        gdCurrentGameStatus = IWGAME_STATUS_RUNNING;
//        return;
//    }
    
    // Update view position
    if (gdIsTouched) {
        
        if (IWPointInRectangle(gdTouchPoint, IWRectangleMake(0.4, 0.4, 0.6, 0.6))) {
            IWPlayerPrintData(&gdPlayerData);
            printf("gdSkyBox.transitionTime = %.2f;\n", gdSkyBoxController.transitionTime);
            gdScreenShotUIMode = gdScreenShotUIMode ? 0 : 1;
            gdIsTouched = false;
        } else {
            float positionScaleFactor = timeSinceLastUpdate * 100.0;
            float directionScaleFactor = timeSinceLastUpdate * 100.0;
            
            gdSkyBoxController.transitionTime += IWUISliderUpdateWithTouch(&gdScreenShotSliderE, gdTouchPoint).x * positionScaleFactor * 100.0;
            gdSkyBoxController.transitionTime = gdSkyBoxController.transitionTime < 0.0 ? 0.0 : gdSkyBoxController.transitionTime;
            gdSkyBoxController.transitionTime = gdSkyBoxController.transitionTime > gdSkyBoxController.colorTransitionTime ? gdSkyBoxController.colorTransitionTime : gdSkyBoxController.transitionTime;
            
            if (gdScreenShotUIMode == 0 ) {
                gdPlayerData.position.x += IWUISliderUpdateWithTouch(&gdScreenShotSliderX, gdTouchPoint).x * positionScaleFactor;
                gdPlayerData.position.y += IWUISliderUpdateWithTouch(&gdScreenShotSliderY, gdTouchPoint).y * positionScaleFactor;
                gdPlayerData.position.z += IWUISliderUpdateWithTouch(&gdScreenShotSliderZ, gdTouchPoint).y * positionScaleFactor;
            } else if (gdScreenShotUIMode == 1 ) {
                
                gdScreenShotRotation.y += IWUISliderUpdateWithTouch(&gdScreenShotSliderX, gdTouchPoint).x * directionScaleFactor;
                gdScreenShotRotation.x += IWUISliderUpdateWithTouch(&gdScreenShotSliderY, gdTouchPoint).y * directionScaleFactor;
                gdScreenShotRotation.z += IWUISliderUpdateWithTouch(&gdScreenShotSliderZ, gdTouchPoint).y * directionScaleFactor;
                
                // Update direction
                IWVector3 dirGLV = gdPlayerDataStart.direction;
                IWVector3 upGLV = gdPlayerDataStart.up;
                
                IWMatrix4 yRotationUpdateMatrix = IWMatrix4MakeRotation(gdScreenShotRotation.y,
                                                                        upGLV.x, upGLV.y, upGLV.z);
                
                IWVector3 normGLV = IWVector3CrossProduct(dirGLV, upGLV);
                
                IWMatrix4 xRotationUpdateMatrix = IWMatrix4MakeRotation(gdScreenShotRotation.x,
                                                                        normGLV.x, normGLV.y, normGLV.z);
                
                IWMatrix4 rotationUpdateMatrix = IWMatrix4Multiply(xRotationUpdateMatrix, yRotationUpdateMatrix);
                
                IWMatrix4 zRotationUpdateMatrix = IWMatrix4MakeRotation(gdScreenShotRotation.z,
                                                                        dirGLV.x, dirGLV.y, dirGLV.z);
                rotationUpdateMatrix = IWMatrix4Multiply(rotationUpdateMatrix, zRotationUpdateMatrix);
                
                gdPlayerData.direction = IWMatrix4MultiplyVector3(rotationUpdateMatrix, dirGLV);
                gdPlayerData.up = IWMatrix4MultiplyVector3(rotationUpdateMatrix, upGLV);
            }
        }
    } else {
        IWUISliderTouchHasEnded(&gdScreenShotSliderX);
        IWUISliderTouchHasEnded(&gdScreenShotSliderY);
        IWUISliderTouchHasEnded(&gdScreenShotSliderZ);
        IWUISliderTouchHasEnded(&gdScreenShotSliderE);
    }
    
    IWGRingBufferSwitchBuffer(&gdTriangleDoubleBuffer);
    IWGRingBufferSwitchBuffer(&gdTextTriangleDoubleBuffer);
    
    if (IWVector4TransitionUpdate(&gdStartTextFieldColorTransition, timeSinceLastUpdate)) {
        IWVector4TransitionReverseAndStart(&gdStartTextFieldColorTransition);
    }
    gdStartTextField.color = gdStartTextFieldColorTransition.currentVector;
    IWGTextFieldSetText(&gdStartTextField, gdStartTextField.text);
    IWGRingBufferSubData(&gdTextTriangleDoubleBuffer,
                          (gdTitleTextField.triangleBufferData.size + gdVersionTextField.triangleBufferData.size) * sizeof(GLfloat),
                          gdStartTextField.triangleBufferData.size * sizeof(GLfloat),
                          gdStartTextField.triangleBufferData.startCPU,
                          false);
    
    IWGSkyBoxControllerUpdate(&gdSkyBoxController, 0.0, &gdPlayerData, true);
    
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


void IWGameTutorialHandler(float timeSinceLastUpdate, float aspectRatio)
{
    IWTutorialControllerUpdate(gdTutorialController,
                               &gdScoreCounter, &gdCubeStatus, &gdFuel, &gdPlayerData,
                               gdTouchPoint, &gdIsTouched,
                               &gdControllerDataAccelerometer,
                               timeSinceLastUpdate);

    IWMatrix4 projectionMatrix = IWMatrix4MakePerspective(65.0 * IW_DEG_TO_RAD, aspectRatio, 0.01, 100.0);
    
    // Update grayscale
    glUseProgram(gdSkyboxShaderProgram.programID);
    glUniform2f(glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale"),
                gdTutorialController->grayScaleTransition.currentVector.x,
                gdTutorialController->grayScaleTransition.currentVector.y);
    glUseProgram(gdMainShaderProgram.programID);
    glUniform2f(glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale"),
                gdTutorialController->grayScaleTransition.currentVector.x,
                gdTutorialController->grayScaleTransition.currentVector.y);
    
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


void IWGameRemoveCubeFromBuffer(IWCubeData *cube, IWGRingBufferData *buffer)
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
            
            IWGRingBufferSubData(&gdTriangleDoubleBuffer,
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
    if (!gdGrayScaleTransition.transitionHasFinished) {
        IWVector3TransitionUpdate(&gdGrayScaleTransition, timeSinceLastUpdate);
        glUseProgram(gdSkyboxShaderProgram.programID);
        glUniform2f(glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale"),
                    gdGrayScaleTransition.currentVector.x,
                    gdGrayScaleTransition.currentVector.y);
        glUseProgram(gdMainShaderProgram.programID);
        glUniform2f(glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale"),
                    gdGrayScaleTransition.currentVector.x,
                    gdGrayScaleTransition.currentVector.y);
    }

    // Check if we are game over
    if (gdFuel.currentLevel == 0.0) {
        gdCurrentGameStatus = IWGAME_STATUS_GAME_OVER;
        //
        printf("GAME OVER\n");
        IWScoreCounterPrintScore(&gdScoreCounter);
        //
        gdClearColor = IWVector4Make(0.9, 0.9, 0.9, 1.0);
        gdCurrentGameStatus = IWGAME_STATUS_GAME_OVER;
        //
        gdPauseMenu.presenter.anchorPoint.x = -0.65;
        IWUIMenuPresenterInitTextFields(&gdPauseMenu.presenter, gdPauseMenu.dataBufferStart);
        IWIUMenuPresenterPresentMenu(&gdPauseMenu.presenter, &gdPauseMenu.pages[1]);
        IWGRingBufferSubData(&gdPauseMenu.multiBuffer, 0,
                              gdPauseMenu.dataBufferSize * sizeof(GLfloat),
                              gdPauseMenu.dataBufferStart,
                              true);
        //
        IWVector3TransitionReverseAndStart(&gdGrayScaleTransition);
        //
        gdPushScoreToLeaderboard = true;
        //
        return;
    }
    
    // Check if pause button has been pressed
    if ((gdCurrentGameStatus == IWGAME_STATUS_RUNNING)
        && IWUIRectangleButtonCheckTouch(&gdUserInterfaceController.pauseButton, gdIsTouched, gdTouchPoint)) {
        gdCurrentGameStatus = IWGAME_STATUS_PAUSED;
        gdPauseTime = 0.0;
        IWVector3TransitionReverseAndStart(&gdGrayScaleTransition);
        gdGrayScaleTransition.transitionTime = 0.5;
        //IWVector3TransitionResetAndStart(&gdGrayScaleTransition);
    } else if (gdCurrentGameStatus == IWGAME_STATUS_PAUSED) {
        if (gdIsTouched) {
            int touchN = IWUIMenuPresenterGetTouch(&gdPauseMenu.presenter, gdTouchPoint);
            gdIsTouched = false;
            if (touchN == 0) {
                IWGRendererTearDownGameAssets();
                IWGRendererSetupStartMenuAssets();
                gdCurrentGameStatus = IWGAME_STATUS_START_MENU;
                gdIsTouched = false;
                gdStateSwitchTimer = IWTimerDataMake(0.0, 0.5, false);
                glUseProgram(gdSkyboxShaderProgram.programID);
                glUniform2f(glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale"), 0.0, 0.4);
                glUseProgram(gdMainShaderProgram.programID);
                glUniform2f(glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale"), 0.0, 0.4);
                return;
            } else if (touchN == 1) {
                gdCurrentGameStatus = IWGAME_STATUS_RUNNING;
                gdGrayScaleTransition = gdGrayScaleTransitionDefault; 
            }
        }
    }
    
    if (!gdUserInterfaceController.pauseButton.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdUserInterfaceController.pauseButton.colorTransition, timeSinceLastUpdate);
        gdUserInterfaceController.pauseButton.color = gdUserInterfaceController.pauseButton.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdUserInterfaceController.pauseButton);
        IWGRingBufferSubDataForBufferObject(&gdUserInterfaceController.triangleMultiBuffer,
                                             &gdUserInterfaceController.pauseButton.triangleBuffer, true);
    }
    
    // Skip game logic if we are paused
    if (gdCurrentGameStatus == IWGAME_STATUS_PAUSED) {
        return;
    }
    
    // Update player position
    float speed = gdCurrentGameStatus == IWGAME_STATUS_PAUSED ? 0.0 : gdPlayerData.speed;
    gdPlayerData.position = IWVector3Add(gdPlayerData.position,
                                         IWVector3MultiplyScalar(IWVector3Normalize(gdPlayerData.direction),
                                                                 timeSinceLastUpdate * speed));
    
    float rotationSpeedMax = 80.0 / 180.0 * M_PI * timeSinceLastUpdate;
    
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
    
    //if (gdControllerDataAccelerometer.rotationSpeed.z != 0.0) {
        IWMatrix4 zRotationUpdateMatrix =
            IWMatrix4MakeRotation(gdControllerDataAccelerometer.rotationSpeed.z * rotationSpeedMax,
                                  dirGLV.x, dirGLV.y, dirGLV.z);
        rotationUpdateMatrix = IWMatrix4Multiply(rotationUpdateMatrix, zRotationUpdateMatrix);
    //}
    
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
    
    // Update total run time
    gdTotalRunTime += timeSinceLastUpdate;
    
    // Update sky
    IWGSkyBoxControllerUpdate(&gdSkyBoxController, timeSinceLastUpdate, &gdPlayerData, true);
    
    // Udpate score and score display
    gdScoreCounter.runningTimeTotal += timeSinceLastUpdate;

    gdZMax = IW_MAX(gdZMax, gdPlayerData.position.z);
    gdScoreCounter.zMax = gdZMax;

    IWScoreCounterUpdateScore(&gdScoreCounter);

    // Update overdrive
    IWPlayerUpdateOverdrive(&gdPlayerData, timeSinceLastUpdate);
    
    // Update fuel
    if (gdTotalRunTime > 5.0 && !gdPlayerData.overdrive) {
        IWFuelRemoveFuel(&gdFuel, 0.05 / 0.6 * timeSinceLastUpdate);
        gdFuel.isWarning = gdFuel.currentLevel / gdFuel.currentMaxLevel < gdFuel.warningLevel ? true : false;
    }
    
    // Update user interface
    IWUserInterfaceControllerUpdate(&gdUserInterfaceController,
                                    &gdScoreCounter,
                                    &gdCubeStatus,
                                    &gdFuel,
                                    &gdPlayerData,
                                    timeSinceLastUpdate);

    // Switch main draw buffer
    IWGRingBufferSwitchBuffer(&gdTriangleDoubleBuffer);
    
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
        
        gdCubeStatus.nGridCubes += gdPoolCubeIndexList.nEntries;
        gdCubeStatus.nPoolCubes = 0;
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
        
        gdCubeStatus.nGridCubes -= 1;
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

                    gdScoreCounter.nGridCubesConverted++;
                    gdCubeStatus.nGridCubes -= 1;
                    gdCubeStatus.nBridgeCubes += 1;                    

                } else if (gdCubeData[i].type == IWCUBE_TYPE_OVERDRIVE) {
                    
                    // Activate overdrive
                    IWPlayerActivatOverdrive(&gdPlayerData);
                    IWPlayerUpdateOverdrive(&gdPlayerData, 0.0);
                    
                    //IWFuelUpdateColor(&gdFuel, IWUI_COLOR_GOLD(0.4), IWFUEL_COLOR_CURRENT, false);
                    //gdOverdriveColorTransition.currentColor = IWUI_COLOR_GOLD(0.4);
                    //gdOverdriveColorTransition.currentTransitionTime = 0.0;
                    //gdOverdriveColorTransition.transitionHasFinished = false;
                    
                    gdCubeData[i].type = IWCUBE_TYPE_POPPING;
                    
                    IWVector3 dimensions = gdCubeData[i].dimensions;
                    gdCubeData[i].positionTransition = IWVector3TransitionMake(dimensions,
                                                                               IWVector3MultiplyScalar(dimensions, 0.01),
                                                                               dimensions, 0.15, 0.0, false, false);
                    
                    gdCubeStatus.nBridgeCubes -= 1;
                    gdCubeStatus.nPoolCubes += 1;
                }
            }
        } else if (!gdCubeData[i].positionTransition.transitionHasFinished) {
            
            IWVector3TransitionUpdate(&gdCubeData[i].positionTransition, timeSinceLastUpdate);
            
            bool updateBuffer = true;
            
            if (gdCubeData[i].type == IWCUBE_TYPE_TRANSITION
                || gdCubeData[i].type == IWCUBE_TYPE_SPAWNING) {
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
            } else if (gdCubeData[i].type == IWCUBE_TYPE_POPPING) {
                if (gdCubeData[i].positionTransition.transitionHasFinished) {
                    gdCubeData[i].type = IWCUBE_TYPE_POOL;
                    
                    gdScoreCounter.nBridgeCubesCollected++;
                    
                    IWIndexListAppendObjectId(&gdPoolCubeIndexList, i);
                    
                    // Remove cube
                    IWGameRemoveCubeFromBuffer(&gdCubeData[i], &gdTriangleDoubleBuffer);
                    updateBuffer = false;
                    gdCubeData[i].dimensions = gdCubeData[i].positionTransition.startVector;
                } else {
                    gdCubeData[i].dimensions = gdCubeData[i].positionTransition.currentVector;
                }
            }
            
            if (updateBuffer) {
                IWCubeToTriangles(&gdCubeData[i]);
                IWGRingBufferSubData(&gdTriangleDoubleBuffer,
                                       gdCubeData[i].triangleBufferData.size * gdCubeData[i].triangleBufferData.bufferIDGPU * sizeof(GLfloat),
                                       gdCubeData[i].triangleBufferData.size * sizeof(GLfloat),
                                       gdCubeData[i].triangleBufferData.startCPU,
                                       false);
            }
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Setup view matrices
    float viewOpeningAngle = 70.0;// before 65.0
    if (gdPlayerData.overdrive) {
        //viewOpeningAngle = 75.0;
    }
    IWMatrix4 projectionMatrix = IWMatrix4MakePerspective(viewOpeningAngle * IW_DEG_TO_RAD, aspectRatio, 0.01, 100.0);
    
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