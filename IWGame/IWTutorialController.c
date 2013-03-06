//
//  IWTutorialController.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWTutorialController.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWTutorialControllerData* IWTutorialControllerMakeDefault(float screenAspectRatio,
                                                          IWGFontMapData *fontMap)
{
    IWTutorialControllerData *tutorialController = malloc(sizeof(IWTutorialControllerData));
    
    tutorialController->tutorialRadius = 2.0;
    tutorialController->currentStage = 0;
    tutorialController->nStages = 7;
    tutorialController->hasFinished = false;
    
    tutorialController->transitionInteractionBlockTimer = IWTimerDataMake(0.0, 1.0, false);
    tutorialController->screenAspectRatio = screenAspectRatio;
    tutorialController->fontMap = fontMap;
    
    tutorialController->mainShaderProgram = NULL;
    tutorialController->skyShaderProgram = NULL;
    tutorialController->textShaderProgram = NULL;
    tutorialController->uiShaderProgram = NULL;
    tutorialController->fontMapTextureData = NULL;
    
    tutorialController->stages[0] = IWTutorialStageMake("Movement",
                                                        "You advance automatically.\n \nTo change direction tilt\nyour device forward/backward\nand to the left/right.",
                                                        true,
                                                        IWUSERINTERFACE_ELEMENT_HUD,
                                                        5, false, false, false, false, false,
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    tutorialController->stages[1] = IWTutorialStageMake("Fuel",
                                                        "You are constantly using up\nfuel. The fuel status is\ndisplayed in the upper left.\n \nTo refill, approach a cube.",
                                                        true,
                                                        IWUSERINTERFACE_ELEMENT_ENERGY_BAR
                                                        | IWUSERINTERFACE_ELEMENT_HUD,
                                                        5, true, false, true, false, false,
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    
    tutorialController->stages[2] = IWTutorialStageMake("Cubes",
                                                        "Cubes come in two flavors:\n 1. Grey cubes\n 2. Red cubes\n \nApproach cubes to trigger\ndifferent actions.",
                                                        false,
                                                        IWUSERINTERFACE_ELEMENT_ENERGY_BAR
                                                        | IWUSERINTERFACE_ELEMENT_HUD,
                                                        5, true, false, true, false, false,
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    
    
    tutorialController->stages[3] = IWTutorialStageMake("Grey/Grid",
                                                        "Grey cubes are called grid\ncubes. On approach grid\ncubes provide fuel and\nconvert into red cubes,\ncalled bridge cubes.",
                                                        false,
                                                        IWUSERINTERFACE_ELEMENT_ENERGY_BAR
                                                        | IWUSERINTERFACE_ELEMENT_HUD,
                                                        5, true, false, true, false, false,
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    
    tutorialController->stages[4] = IWTutorialStageMake("Red/Bridge",
                                                        "Red cubes provide a\ntemporary speed boost.\n \nThey are arranged in a\nlong line to enable you\nto cross large distances.",
                                                        true,
                                                        IWUSERINTERFACE_ELEMENT_ENERGY_BAR
                                                        | IWUSERINTERFACE_ELEMENT_HUD
                                                        | IWUSERINTERFACE_ELEMENT_CUBE_COUNTER,
                                                        5, true, false, true, true, true,
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    
    tutorialController->stages[5] = IWTutorialStageMake("Disappearance",
                                                        "Grid cubes disappear with\ntime, so better be quick\nwith converting them.\nBridge cubes are stable and\nprovide new grid cubes when\napproached/collected.",
                                                        false,
                                                        IWUSERINTERFACE_ELEMENT_ENERGY_BAR
                                                        | IWUSERINTERFACE_ELEMENT_HUD
                                                        | IWUSERINTERFACE_ELEMENT_CUBE_COUNTER,
                                                        5, true, false, true, true, true,
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    
    
    tutorialController->stages[6] = IWTutorialStageMake("Spawning",
                                                        "If enough bridge cubes have\nbeen collected, a new set\nof grid cubes will automat-\nically spawn, when you reach\nthe end of a bridge cube\nline.",
                                                        false,
                                                        IWUSERINTERFACE_ELEMENT_ENERGY_BAR
                                                        | IWUSERINTERFACE_ELEMENT_HUD
                                                        | IWUSERINTERFACE_ELEMENT_CUBE_COUNTER,
                                                        5, true, false, true, true, true,
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    
    tutorialController->grayScaleTransitionBase =
    IWVector3TransitionMake(IWVector3Make(1.0, 0.2, 0.0),
                            IWVector3Make(0.0, 0.2, 0.0),
                            IWVector3Make(1.0, 0.2, 0.0),
                            1.0, 0.0, false, false);
    tutorialController->grayScaleTransition = tutorialController->grayScaleTransitionBase;
    
    tutorialController->grayScaleTransition.transitionHasFinished = true;
    
    // Make button rectangles
    tutorialController->startNextButtonRectangle = IWRectangleMake(0.0, 0.0, 0.2, 0.3);
    tutorialController->skipTutorialButtonRectangle = IWRectangleMake(0.0, 0.7, 0.2, 1.0);
    
    // Allocate memory for text buffer
    tutorialController->textDataBufferSize = (1 * 6 + 1 * 7 + 1 * 15 + 6 * 30) * 6 * 9;
    tutorialController->textDataBufferStart = malloc(tutorialController->textDataBufferSize * sizeof(GLfloat));
    
    tutorialController->textMultiBuffer = IWGRingBufferGen();
    
    GLuint bufferOffset = 0;
    
    tutorialController->textColorText = IWVector4Make(0.2, 0.2, 0.2, 0.8);
    tutorialController->textColorRunning = IWVector4Make(1.0, 1.0, 1.0, 0.8);
    
    float textBorderOffset = 0.1;
    tutorialController->skipTutorialButton
    = IWGTextFieldMake(IWVector2Make(1.0 - textBorderOffset * screenAspectRatio, -1.0 + textBorderOffset),
                       IWGEOMETRY_ANCHOR_POSITION_LOWER_RIGHT,
                       1, 6,
                       1. / screenAspectRatio,
                       "[SKIP]",
                       0.18, 0.0,
                       IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                       tutorialController->textColorText,
                       fontMap,
                       tutorialController->textDataBufferStart);
    bufferOffset += tutorialController->skipTutorialButton.triangleBufferData.size;
    
    tutorialController->startNextButton
    = IWGTextFieldMake(IWVector2Make(-1.0 + textBorderOffset * screenAspectRatio, -1.0 + textBorderOffset),
                       IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
                       1, 7,
                       1. / screenAspectRatio,
                       "[START]",
                       0.18, 0.0,
                       IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                       tutorialController->textColorText,
                       fontMap,
                       tutorialController->textDataBufferStart
                       + bufferOffset);
    bufferOffset += tutorialController->startNextButton.triangleBufferData.size;
    
    tutorialController->stageTitle
    = IWGTextFieldMake(IWVector2Make(-1.0 + textBorderOffset * screenAspectRatio, 1.0 - textBorderOffset),
                       IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                       1, 15,
                       1. / screenAspectRatio,
                       tutorialController->stages[0].title,
                       0.26, 0.,
                       IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                       tutorialController->textColorText,
                       fontMap,
                       tutorialController->textDataBufferStart
                       + bufferOffset);
    bufferOffset += tutorialController->stageTitle.triangleBufferData.size;
    
    tutorialController->stageText
    = IWGTextFieldMake(IWVector2Make(-1.0 + textBorderOffset * screenAspectRatio,
                                     1.0 - textBorderOffset - 0.26 - 0.1),
                       IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                       6, 30,
                       1. / screenAspectRatio,
                       tutorialController->stages[0].text,
                       0.16, 0.0,
                       IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                       tutorialController->textColorText,
                       fontMap,
                       tutorialController->textDataBufferStart
                       + bufferOffset);
    bufferOffset += tutorialController->stageText.triangleBufferData.size;
    
    // Cubes
    unsigned int nCubesPerAxis = tutorialController->stages[0].nCubesPerAxis;
    tutorialController->cubeController = IWCubeControllerMake(nCubesPerAxis, nCubesPerAxis, nCubesPerAxis,
                                                              .04, .12, IWVector3Make(0.0, 0.0, 0.0),
                                                              IWVector4Make(0.5, 0.5, 0.5, 1.0),
                                                              0, 0.05,
                                                              tutorialController->stages[0].cubesInteractive,
                                                              tutorialController->stages[0].removeCubes, 1.0, 0);
    
    // Sky box
    tutorialController->skyBoxController = IWGSkyBoxControllerMakeDefault();
    
    // User interface
    tutorialController->userInterfaceController = IWUserInterfaceControllerMake(screenAspectRatio,
                                                                                IWUSERINTERFACE_ELEMENT_HUD,
                                                                                fontMap);
    
    return tutorialController;
}

void IWTutorialControllerSetupVBOs(IWTutorialControllerData *tutorialController,
                                   const IWGShaderProgramData *mainShaderProgram,
                                   const IWGShaderProgramData *skyShaderProgram,
                                   const IWGShaderProgramData *uiShaderProgram,
                                   const IWGShaderProgramData *textShaderProgram,
                                   GLuint textureHandlerId,
                                   GLvoid* fontMapTextureData)
{
    // Save programs for later use
    tutorialController->mainShaderProgram = mainShaderProgram;
    tutorialController->skyShaderProgram = skyShaderProgram;
    tutorialController->textShaderProgram = textShaderProgram;
    tutorialController->uiShaderProgram = uiShaderProgram;
    tutorialController->fontMapTextureData = fontMapTextureData;
    tutorialController->textureHandlerId = textureHandlerId;
    
    // Text buffer
    glUseProgram(textShaderProgram->programID);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        tutorialController->textMultiBuffer.nVertices[i] = tutorialController->textDataBufferSize / 9;
        
        IWGRingBufferBind(&tutorialController->textMultiBuffer, i);
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontMapTextureData);
        
        glBufferData(GL_ARRAY_BUFFER,
                     tutorialController->textDataBufferSize * sizeof(GLfloat),
                     tutorialController->textDataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(textShaderProgram->vertexSlot);
        glVertexAttribPointer(textShaderProgram->vertexSlot, 3,
                              GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(textShaderProgram->colorSlot);
        glVertexAttribPointer(textShaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE,
                              9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textShaderProgram->textureOffsetSlot);
        glVertexAttribPointer(textShaderProgram->textureOffsetSlot, 2, GL_FLOAT,
                              GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    
    glBindVertexArrayOES(0);
    
    // Cubes
    IWCubeControllerSetupVBOs(&tutorialController->cubeController,
                              mainShaderProgram);
    
    // Sky box
    IWGSkyBoxControllerFillVBO(&tutorialController->skyBoxController,
                               skyShaderProgram);
    
    // User interface
    IWUserInterfaceControllerSetupVBOs(&tutorialController->userInterfaceController,
                                       uiShaderProgram,
                                       textShaderProgram,
                                       textureHandlerId,
                                       fontMapTextureData);
    return;
}

void IWTutorialControllerUpdate(IWTutorialControllerData *tutorialController,
                                IWScoreCounterData *scoreCounter,
                                IWCubeStatusData *cubeStatus,
                                IWFuel *fuel,
                                IWPlayerData *player,
                                IWPoint2D touchPoint,
                                bool *isTouched,
                                IWControllerData *controller,
                                float timeSinceLastUpdate)
{
    IWTutorialStageData *currentStage = &tutorialController->stages[tutorialController->currentStage];
    
    IWTimerUpdate(&tutorialController->transitionInteractionBlockTimer, timeSinceLastUpdate);
    
    if (tutorialController->transitionInteractionBlockTimer.hasFinished
        && *isTouched
        && !tutorialController->hasFinished) {
        
        if (IWPointInRectangle(touchPoint, tutorialController->startNextButtonRectangle)) {
            if (currentStage->status == IWTUTORIALSTAGE_STATUS_TEXT
                && currentStage->hasActionStage) {
                
                currentStage->status = IWTUTORIALSTAGE_STATUS_RUNNING;
                
                IWGTextFieldSetText(&tutorialController->stageText, "");
                IWGTextFieldSetText(&tutorialController->stageTitle, "");
                tutorialController->startNextButton.color = tutorialController->textColorRunning;
                IWGTextFieldSetText(&tutorialController->startNextButton, "[NEXT]");
                tutorialController->skipTutorialButton.color = tutorialController->textColorRunning;
                IWGTextFieldSetText(&tutorialController->skipTutorialButton, tutorialController->skipTutorialButton.text);
                
                IWGRingBufferSubData(&tutorialController->textMultiBuffer, 0,
                                     tutorialController->textDataBufferSize * sizeof(GLfloat),
                                     tutorialController->textDataBufferStart,
                                     true);
                
                tutorialController->grayScaleTransition = tutorialController->grayScaleTransitionBase;
                
                *isTouched = false;
                IWTimerResetAndStart(&tutorialController->transitionInteractionBlockTimer);
                
            } else if ((currentStage->status == IWTUTORIALSTAGE_STATUS_TEXT
                        && !currentStage->hasActionStage)
                       || currentStage->status == IWTUTORIALSTAGE_STATUS_RUNNING) {
                
                tutorialController->currentStage += 1;
                
                if (tutorialController->currentStage < tutorialController->nStages) {
                    
                    if (currentStage->status == IWTUTORIALSTAGE_STATUS_RUNNING) {
                        tutorialController->grayScaleTransition = tutorialController->grayScaleTransitionBase;
                        tutorialController->grayScaleTransition.transitionTime = 0.2;
                        IWVector3TransitionReverseAndStart(&tutorialController->grayScaleTransition);
                    }
                    
                    currentStage = &tutorialController->stages[tutorialController->currentStage];
                    
                    // User interface
                    IWUserInterfacePurgeData(&tutorialController->userInterfaceController);
                    tutorialController->userInterfaceController =
                    IWUserInterfaceControllerMake(tutorialController->screenAspectRatio,
                                                  currentStage->userInterfaceElements,
                                                  tutorialController->fontMap);
                    IWUserInterfaceControllerSetupVBOs(&tutorialController->userInterfaceController,
                                                       tutorialController->uiShaderProgram,
                                                       tutorialController->textShaderProgram,
                                                       tutorialController->textureHandlerId,
                                                       tutorialController->fontMapTextureData);
                    
                    // Cubes
                    if (currentStage->hasActionStage) {
                        IWCubeControllerPurgeData(&tutorialController->cubeController);
                        unsigned int nCubesPerAxis = currentStage->nCubesPerAxis;
                        tutorialController->cubeController =
                        IWCubeControllerMake(nCubesPerAxis, nCubesPerAxis, nCubesPerAxis,
                                             .04, .12, IWVector3Make(0.0, 0.0, 0.0),
                                             IWVector4Make(0.5, 0.5, 0.5, 1.0),
                                             0, 0.05,
                                             currentStage->cubesInteractive,
                                             currentStage->removeCubes,
                                             1.0, 0);
                        IWCubeControllerSetupVBOs(&tutorialController->cubeController,
                                                  tutorialController->mainShaderProgram);
                        *cubeStatus = IWCubeStatusMakeEmpty();
                        cubeStatus->nGridCubes = tutorialController->cubeController.nCubes;
                    }
                    //
                    IWGTextFieldSetText(&tutorialController->stageText, currentStage->text);
                    IWGTextFieldSetText(&tutorialController->stageTitle, currentStage->title);
                    tutorialController->startNextButton.color = tutorialController->textColorText;
                    IWGTextFieldSetText(&tutorialController->startNextButton,
                                        currentStage->hasActionStage ? "[START]" : "[NEXT]");
                    tutorialController->skipTutorialButton.color = tutorialController->textColorText;
                    IWGTextFieldSetText(&tutorialController->skipTutorialButton, tutorialController->skipTutorialButton.text);
                    
                    IWGRingBufferSubData(&tutorialController->textMultiBuffer, 0,
                                         tutorialController->textDataBufferSize * sizeof(GLfloat),
                                         tutorialController->textDataBufferStart,
                                         true);
                    
                    *player = currentStage->player;

                } else {
                    tutorialController->hasFinished = true;
                    tutorialController->grayScaleTransition = tutorialController->grayScaleTransitionBase;
                    IWVector3TransitionReverseAndStart(&tutorialController->grayScaleTransition);
                }
                *isTouched = false;
                IWTimerResetAndStart(&tutorialController->transitionInteractionBlockTimer);
            }
        }
    }
    
    // Process grayscale shader transitions
    if (!tutorialController->grayScaleTransition.transitionHasFinished) {
        IWVector3TransitionUpdate(&tutorialController->grayScaleTransition, timeSinceLastUpdate);
    }
    
    // Check if player is inside the tutorial zone
    if (IWVector3Length(player->position) > tutorialController->tutorialRadius) {
        IWCubeControllerPurgeData(&tutorialController->cubeController);
        unsigned int nCubesPerAxis = currentStage->nCubesPerAxis;
        tutorialController->cubeController =
        IWCubeControllerMake(nCubesPerAxis, nCubesPerAxis, nCubesPerAxis,
                             .04, .12, IWVector3Make(0.0, 0.0, 0.0),
                             IWVector4Make(0.5, 0.5, 0.5, 1.0),
                             0, 0.05,
                             currentStage->cubesInteractive,
                             currentStage->removeCubes,
                             1.0, 0);
        IWCubeControllerSetupVBOs(&tutorialController->cubeController,
                                  tutorialController->mainShaderProgram);
        *cubeStatus = IWCubeStatusMakeEmpty();
        cubeStatus->nGridCubes = tutorialController->cubeController.nCubes;
        *player = currentStage->player;
        tutorialController->grayScaleTransition = tutorialController->grayScaleTransitionBase;
    }
    
    // If we are running, process game logic
    if (currentStage->status == IWTUTORIALSTAGE_STATUS_RUNNING
        && !tutorialController->hasFinished) {
        // Update player position
        float speed = player->speed;
        player->position = IWVector3Add(player->position,
                                        IWVector3MultiplyScalar(IWVector3Normalize(player->direction),
                                                                timeSinceLastUpdate * speed));
        
        float rotationSpeedMax = 80.0 / 180.0 * M_PI * timeSinceLastUpdate;
        
        // Update direction
        IWVector3 dirGLV = IWVector3Make(player->direction.x, player->direction.y, player->direction.z);
        IWVector3 upGLV = IWVector3Make(player->up.x, player->up.y, player->up.z);
        
        IWMatrix4 yRotationUpdateMatrix =
        IWMatrix4MakeRotation(controller->rotationSpeed.y * rotationSpeedMax,
                              upGLV.x, upGLV.y, upGLV.z);
        
        IWVector3 normGLV = IWVector3CrossProduct(dirGLV, upGLV);
        
        IWMatrix4 xRotationUpdateMatrix =
        IWMatrix4MakeRotation(controller->rotationSpeed.x * rotationSpeedMax,
                              normGLV.x, normGLV.y, normGLV.z);
        
        IWMatrix4 rotationUpdateMatrix = IWMatrix4Multiply(xRotationUpdateMatrix, yRotationUpdateMatrix);
        
        IWMatrix4 zRotationUpdateMatrix =
        IWMatrix4MakeRotation(controller->rotationSpeed.z * rotationSpeedMax,
                              dirGLV.x, dirGLV.y, dirGLV.z);
        rotationUpdateMatrix = IWMatrix4Multiply(rotationUpdateMatrix, zRotationUpdateMatrix);
        
        player->direction = IWMatrix4MultiplyVector3(rotationUpdateMatrix, dirGLV);
        player->up = IWMatrix4MultiplyVector3(rotationUpdateMatrix, upGLV);
        
        // Update cubes
        IWCubeControllerUpdate(&tutorialController->cubeController,
                               scoreCounter, cubeStatus, fuel, player,
                               timeSinceLastUpdate);
        
        // Update overdrive
        IWPlayerUpdateOverdrive(player, timeSinceLastUpdate);
        
        // Update fuel
        if (currentStage->removeFuel && !player->overdrive) {
            IWFuelRemoveFuel(fuel, 0.05 / 0.6 * timeSinceLastUpdate);
            fuel->isWarning = fuel->currentLevel / fuel->currentMaxLevel < fuel->warningLevel ? true : false;
        }
        // Collision detection
        for (int i = 0; i < tutorialController->cubeController.nCubes; i++) {
            if (tutorialController->cubeController.cubeData[i].isInteractive) {
                if (IWVector3DistanceSquared(player->position, tutorialController->cubeController.cubeData[i].centerPosition)
                    < tutorialController->cubeController.cubeData[i].collisionRadius * tutorialController->cubeController.cubeData[i].collisionRadius) {
                    // Collision detected
                    IWFuelAddFuel(fuel, fuel->currentMaxLevel);
                    
                    tutorialController->cubeController.cubeData[i].isInteractive = false;
                    
                    if (currentStage->convertGridToBridge &&
                        tutorialController->cubeController.cubeData[i].type == IWCUBE_TYPE_STANDARD) {
                        
                        // Remove from standard cube list
                        IWIndexListReplaceWithLast(&tutorialController->cubeController.standardCubeIndexList,
                                                   IWIndexListGetIndexForObjectId(&tutorialController->cubeController.standardCubeIndexList, i));
                        
                        // Move cube
                        tutorialController->cubeController.cubeData[i].type = IWCUBE_TYPE_TRANSITION;
                        
                        IWVector3 newPosition = tutorialController->cubeController.secondaryPositions[tutorialController->cubeController.secondaryPositionCounter];
                        float transitionTime = IWVector3Distance(newPosition, tutorialController->cubeController.cubeData[i].centerPosition) / 0.25;
                        tutorialController->cubeController.secondaryPositionCounter++;
                        tutorialController->cubeController.cubeData[i].positionTransition
                            = IWVector3TransitionMake(tutorialController->cubeController.cubeData[i].centerPosition,
                                                      newPosition,
                                                      tutorialController->cubeController.cubeData[i].centerPosition,
                                                      transitionTime, 0.0, false, false);
                        IWGPrimitiveBufferDataUpdateColor(&tutorialController->cubeController.cubeData[i].triangleBufferData, IWUI_COLOR_RED(1.0));
                        tutorialController->cubeController.cubeData[i].color = IWUI_COLOR_DARK_RED(1.0);
                        
                        //gdScoreCounter.nGridCubesConverted++;
                        cubeStatus->nGridCubes -= 1;
                        cubeStatus->nBridgeCubes += 1;
                        
                    } else if (!currentStage->convertGridToBridge
                               || tutorialController->cubeController.cubeData[i].type == IWCUBE_TYPE_OVERDRIVE) {
                        
                        // Activate overdrive
                        if (currentStage->activateOverdrive) {
                            IWPlayerActivatOverdrive(player);
                            IWPlayerUpdateOverdrive(player, 0.0);
                        }
                        
                        tutorialController->cubeController.cubeData[i].type = IWCUBE_TYPE_POPPING;
                        
                        IWVector3 dimensions = tutorialController->cubeController.cubeData[i].dimensions;
                        tutorialController->cubeController.cubeData[i].positionTransition
                            = IWVector3TransitionMake(dimensions,
                                                      IWVector3MultiplyScalar(dimensions, 0.01),
                                                      dimensions, 0.15, 0.0, false, false);
                        
                        cubeStatus->nBridgeCubes -= 1;
                        cubeStatus->nPoolCubes += 1;
                    }
                }
            } else if (!tutorialController->cubeController.cubeData[i].positionTransition.transitionHasFinished) {
                
                IWVector3TransitionUpdate(&tutorialController->cubeController.cubeData[i].positionTransition, timeSinceLastUpdate);
                
                bool updateBuffer = true;
                
                if (tutorialController->cubeController.cubeData[i].type == IWCUBE_TYPE_TRANSITION
                    || tutorialController->cubeController.cubeData[i].type == IWCUBE_TYPE_SPAWNING) {
                    if (tutorialController->cubeController.cubeData[i].positionTransition.transitionHasFinished) {
                        tutorialController->cubeController.cubeData[i].centerPosition = tutorialController->cubeController.cubeData[i].positionTransition.endVector;
                        if (tutorialController->cubeController.cubeData[i].type == IWCUBE_TYPE_TRANSITION) {
                            // Cube has arrived at bridge position
                            //IWGPrimitiveBufferDataUpdateColor(&tutorialController->cubeController.cubeData[i].triangleBufferData, IWUI_COLOR_GOLD(1.0));
                            tutorialController->cubeController.cubeData[i].color = IWUI_COLOR_RED(1.0);
                            tutorialController->cubeController.cubeData[i].isInteractive = true;
                            tutorialController->cubeController.cubeData[i].type = IWCUBE_TYPE_OVERDRIVE;
                        } else {
                            //tutorialController->cubeController.cubeData[i].color = IWUI_COLOR_BLUE(1.0);
                            tutorialController->cubeController.cubeData[i].isInteractive = true;
                            tutorialController->cubeController.cubeData[i].type = IWCUBE_TYPE_STANDARD;
                        }
                    } else {
                        // Continue moving cube
                        tutorialController->cubeController.cubeData[i].centerPosition = tutorialController->cubeController.cubeData[i].positionTransition.currentVector;
                    }
                } else if (tutorialController->cubeController.cubeData[i].type == IWCUBE_TYPE_POPPING) {
                    if (tutorialController->cubeController.cubeData[i].positionTransition.transitionHasFinished) {
                        tutorialController->cubeController.cubeData[i].type = IWCUBE_TYPE_POOL;
                        
                        //gdScoreCounter.nBridgeCubesCollected++;
                        
                        IWIndexListAppendObjectId(&tutorialController->cubeController.poolCubeIndexList, i);
                        
                        // Remove cube
                        IWCubeControllerRemoveCube(&tutorialController->cubeController, &tutorialController->cubeController.cubeData[i]);
                        //IWGameRemoveCubeFromBuffer(&tutorialController->cubeController.cubeData[i], &gdTriangleDoubleBuffer);
                        updateBuffer = false;
                        tutorialController->cubeController.cubeData[i].dimensions = tutorialController->cubeController.cubeData[i].positionTransition.startVector;
                    } else {
                        tutorialController->cubeController.cubeData[i].dimensions = tutorialController->cubeController.cubeData[i].positionTransition.currentVector;
                    }
                }
                
                if (updateBuffer) {
                    IWCubeToTriangles(&tutorialController->cubeController.cubeData[i]);
                    IWGRingBufferSubData(&tutorialController->cubeController.triangleRingBuffer,
                                         tutorialController->cubeController.cubeData[i].triangleBufferData.size * tutorialController->cubeController.cubeData[i].triangleBufferData.bufferIDGPU * sizeof(GLfloat),
                                         tutorialController->cubeController.cubeData[i].triangleBufferData.size * sizeof(GLfloat),
                                         tutorialController->cubeController.cubeData[i].triangleBufferData.startCPU,
                                         true);
                }
            }
        }
        
        // Update skybox
        IWGSkyBoxControllerUpdate(&tutorialController->skyBoxController, timeSinceLastUpdate, player, true);
        
        // Update user interface
        IWUserInterfaceControllerUpdate(&tutorialController->userInterfaceController,
                                        scoreCounter, cubeStatus, fuel, player,
                                        timeSinceLastUpdate);
        
    } else if (!tutorialController->hasFinished) {
        *player = currentStage->player;
    }
    
    return;
}

void IWTutorialControllerRender(IWTutorialControllerData *tutorialController,
                                const IWGShaderProgramData *mainShaderProgram,
                                const IWGShaderProgramData *skyShaderProgram,
                                const IWGShaderProgramData *uiShaderProgram,
                                const IWGShaderProgramData *textShaderProgram)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mainShaderProgram->programID);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    IWCubeControllerRender(&tutorialController->cubeController, mainShaderProgram->programID);
    
    glDisable(GL_CULL_FACE);
    
    glUseProgram(skyShaderProgram->programID);
    
    IWGSkyBoxControllerRender(&tutorialController->skyBoxController);
    
    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    
    glUseProgram(uiShaderProgram->programID);
    
    if (tutorialController->stages[tutorialController->currentStage].status == IWTUTORIALSTAGE_STATUS_RUNNING
        && !tutorialController->hasFinished) {
        IWUserInterfaceControllerRender(&tutorialController->userInterfaceController,
                                        textShaderProgram->programID,
                                        uiShaderProgram->programID);
    }
    
    if (!tutorialController->hasFinished) {
        glUseProgram(textShaderProgram->programID);
        
        IWGRingBufferBindCurrentDrawBuffer(&tutorialController->textMultiBuffer);
        
        glDrawArrays(GL_TRIANGLES, 0,
                     tutorialController->textMultiBuffer.nVertices[tutorialController->textMultiBuffer.currentDrawBuffer]);
        
        glBindVertexArrayOES(0);
        
        IWGRingBufferSwitchBuffer(&tutorialController->textMultiBuffer);
    }
    
    glDisable(GL_BLEND);
}

void IWTutorialControllerPurgeData(IWTutorialControllerData *tutorialController)
{
    IWCubeControllerPurgeData(&tutorialController->cubeController);
    IWGSkyBoxControllerPurgeData(&tutorialController->skyBoxController);
    IWUserInterfacePurgeData(&tutorialController->userInterfaceController);
    free(tutorialController->textDataBufferStart);
    tutorialController->textDataBufferStart = NULL;
}