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
    
    tutorialController->tutorialRadius = 1.5;
    tutorialController->currentStage = 0;
    tutorialController->nStages = 2;
    tutorialController->hasFinished = false;
    
    tutorialController->transitionInteractionBlockTimer = IWTimerDataMake(0.0, 1.0, false);
    tutorialController->screenAspectRatio = screenAspectRatio;
    tutorialController->fontMap = fontMap;
    
    tutorialController->mainShaderProgram = NULL;
    tutorialController->skyShaderProgram = NULL;
    tutorialController->textShaderProgram = NULL;
    tutorialController->uiShaderProgram = NULL;
    tutorialController->fontMapTextureData = NULL;
    
    tutorialController->stages[0] = IWTutorialStageMake(IWUSERINTERFACE_ELEMENT_HUD,
                                                        5, false, false, false,
                                                        "Movement",
                                                        "You advance automatically.\n \nTo change direction tilt\nyour device forward/backward\nand to the left/right.",
                                                        IWPlayerDataMakeSimple(IWVector3Make(0.0, 0.0, -1.0),
                                                                               IWVector3Make(0.0, 0.0, 1.0),
                                                                               IWVector3Make(0.0, 1.0, 0.0)));
    tutorialController->stages[1] = IWTutorialStageMake(IWUSERINTERFACE_ELEMENT_ENERGY_BAR
                                                        | IWUSERINTERFACE_ELEMENT_HUD,
                                                        5, false, false, false,
                                                        "Fuel",
                                                        "You are constantly loosing\nfuel. The fuel status is\ndisplayed in the upper left.\n \nTo refill, approach a cube.",
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
    tutorialController->textDataBufferSize = (1 * 6 + 1 * 7 + 1 * 12 + 6 * 30) * 6 * 9;
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
                           1, 12,
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
    tutorialController->cubeController = IWCubeControllerMake(5, false, 0.0, false, 0);

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
        && *isTouched) {
        
        if (IWPointInRectangle(touchPoint, tutorialController->startNextButtonRectangle)) {
            if (currentStage->status == IWTUTORIALSTAGE_STATUS_TEXT) {
                
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
                
            } else if (currentStage->status == IWTUTORIALSTAGE_STATUS_RUNNING) {
                
                tutorialController->currentStage += 1;
                
                if (tutorialController->currentStage < tutorialController->nStages) {
                    
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
                    
                    //
                    IWGTextFieldSetText(&tutorialController->stageText, currentStage->text);
                    IWGTextFieldSetText(&tutorialController->stageTitle, currentStage->title);
                    tutorialController->startNextButton.color = tutorialController->textColorText;
                    IWGTextFieldSetText(&tutorialController->startNextButton, "[START]");
                    tutorialController->skipTutorialButton.color = tutorialController->textColorText;
                    IWGTextFieldSetText(&tutorialController->skipTutorialButton, tutorialController->skipTutorialButton.text);
                    
                    IWGRingBufferSubData(&tutorialController->textMultiBuffer, 0,
                                         tutorialController->textDataBufferSize * sizeof(GLfloat),
                                         tutorialController->textDataBufferStart,
                                         true);
                    
                    tutorialController->grayScaleTransition = tutorialController->grayScaleTransitionBase;
                    IWVector3TransitionReverseAndStart(&tutorialController->grayScaleTransition);
                    
                    *player = currentStage->player;
                } else {
                    tutorialController->hasFinished = true;
                }
                *isTouched = false;
                IWTimerResetAndStart(&tutorialController->transitionInteractionBlockTimer);
            }
        }
    }

    if (!tutorialController->grayScaleTransition.transitionHasFinished) {
        IWVector3TransitionUpdate(&tutorialController->grayScaleTransition, timeSinceLastUpdate);
    }
    
    if (IWVector3Length(player->position) > tutorialController->tutorialRadius) {
        *player = currentStage->player;
        tutorialController->grayScaleTransition = tutorialController->grayScaleTransitionBase;
    }
    
    if (currentStage->status == IWTUTORIALSTAGE_STATUS_RUNNING) {
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

    }

    IWCubeControllerUpdate(&tutorialController->cubeController,
                           scoreCounter, cubeStatus, fuel, player,
                           timeSinceLastUpdate);
    
    IWGSkyBoxControllerUpdate(&tutorialController->skyBoxController, timeSinceLastUpdate, player, true);
    
    IWUserInterfaceControllerUpdate(&tutorialController->userInterfaceController,
                                    scoreCounter, cubeStatus, fuel, player,
                                    timeSinceLastUpdate);
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
    
    if (tutorialController->stages[tutorialController->currentStage].status == IWTUTORIALSTAGE_STATUS_RUNNING) {
        IWUserInterfaceControllerRender(&tutorialController->userInterfaceController,
                                        textShaderProgram->programID,
                                        uiShaderProgram->programID);
    }

    glUseProgram(textShaderProgram->programID);
    
    IWGRingBufferBindCurrentDrawBuffer(&tutorialController->textMultiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0,
                 tutorialController->textMultiBuffer.nVertices[tutorialController->textMultiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGRingBufferSwitchBuffer(&tutorialController->textMultiBuffer);
    
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