//
//  IWStartMenuController.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWStartMenuController.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWStartMenuControllerData* IWStartMenuControllerMakeDefault(float screenAspectRatio,
                                                            IWGFontMapData *fontMap)
{
    IWStartMenuControllerData *startMenuController = malloc(sizeof(IWStartMenuControllerData));
    
    startMenuController->transitionInteractionBlockTimer = IWTimerDataMake(0.0, 1.0, false);
    startMenuController->screenAspectRatio = screenAspectRatio;
    startMenuController->fontMap = fontMap;
    
    startMenuController->mainShaderProgram = NULL;
    startMenuController->skyShaderProgram = NULL;
    startMenuController->textShaderProgram = NULL;
    startMenuController->fontMapTextureData = NULL;
    
    // Allocate memory for text buffer
    startMenuController->textDataBufferSize = (2 * 10 + 1 * 9) * 6 * 9;
    startMenuController->textDataBufferStart = malloc(startMenuController->textDataBufferSize * sizeof(GLfloat));
    
    startMenuController->textMultiBuffer = IWGRingBufferGen();
    
    startMenuController->player = IWPlayerDataMakeSimple(IWVector3Make(-0.8, 0.0, -1.),
                                                         IWVector3Normalize(IWVector3Make(0.4, 0.0, 1.0)),
                                                         IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0)));
    
    GLuint bufferOffset = 0;
    
    startMenuController->titleTextField
    = IWGTextFieldMake(IWVector2Make(0.95, 0.96),
                       IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                       2, 10,
                       1. / screenAspectRatio,
                       "ARCHRUNNER\nALPHA",
                       0.18, -0.04,
                       IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                       IWVector4Make(1.0, 1.0, 1.0, 1.0),
                       fontMap,
                       startMenuController->textDataBufferStart
                       + bufferOffset);
    bufferOffset += startMenuController->titleTextField.triangleBufferData.size;
    
    startMenuController->versionTextField
    = IWGTextFieldMake(IWVector2Make(0.98, -0.99),
                       IWGEOMETRY_ANCHOR_POSITION_LOWER_RIGHT,
                       1, 9,
                       1. / screenAspectRatio,
                       VERSION,
                       0.1, 0.0,
                       IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                       IWVector4Make(1.0, 1.0, 1.0, 1.0),
                       fontMap,
                       startMenuController->textDataBufferStart
                       + bufferOffset);
    
    // Menu
    
    startMenuController->menuController
    = IWUIMenuControllerMake(IWUIMenuPresenterMake(2, 12, 1,
                                                   IWVector2Make(0.3, 0.35), 1. / screenAspectRatio,
                                                   0.16, 1.2,
                                                   IWVector4Make(1.0, 1.0, 1.0, 0.8),
                                                   fontMap), 2, true, true);

    startMenuController->menuController.pages[0].title = "";
    startMenuController->menuController.pages[0].isActive = true;
    //IWUIMenuPageAddItem(&gdPauseMenu.pages[0], IWUIMENUITEM_ITEM_TYPE_EMPTY, "", "", 0, NULL, 0);
    IWUIMenuPageAddItem(&startMenuController->menuController.pages[0],
                        IWUIMENUITEM_ITEM_TYPE_ACTION,
                        "  [START]", "", 0, NULL, 1);
    //IWUIMenuPageAddItem(&gdPauseMenu.pages[0], IWUIMENUITEM_ITEM_TYPE_EMPTY, "", "", 0, NULL, 0);
    //IWUIMenuPageAddItem(&startMenuController->menuController.pages[0], IWUIMENUITEM_ITEM_TYPE_ACTION, "[CONTINUE]", "", 0, NULL, 2);
    startMenuController->menuController.pages[1].title = "";
    startMenuController->menuController.pages[1].isActive = false;
    IWUIMenuPageAddItem(&startMenuController->menuController.pages[1], IWUIMENUITEM_ITEM_TYPE_ACTION, "[NEW GAME]", "", 0, NULL, 1);
    IWUIMenuPageAddItem(&startMenuController->menuController.pages[1], IWUIMENUITEM_ITEM_TYPE_ACTION, "[TUTORIAL]", "", 0, NULL, 2);
    
    IWUIMenuPresenterInitTextFields(&startMenuController->menuController.presenter, startMenuController->menuController.dataBufferStart);
    IWIUMenuPresenterPresentMenu(&startMenuController->menuController.presenter, &startMenuController->menuController.pages[0]);
    
    startMenuController->textColorText = IWVector4Make(0.2, 0.2, 0.2, 0.8);
    
    // Cubes
    unsigned int nCubesPerAxis = 8;
    startMenuController->cubeController = IWCubeControllerMake(nCubesPerAxis, nCubesPerAxis, nCubesPerAxis,
                                                               .04, .12, IWVector3Make(0.0, 0.0, 0.0),
                                                               IWVector4Make(0.5, 0.5, 0.5, 1.0),
                                                               1, 0.05,
                                                               false,
                                                               false, 1.0, 0);
    
    // Sky box
    startMenuController->skyBoxController = IWGSkyBoxControllerMakeDefault();
    
    return startMenuController;
}

void IWStartMenuControllerSetupVBOs(IWStartMenuControllerData *startMenuController,
                                    const IWGShaderProgramData *mainShaderProgram,
                                    const IWGShaderProgramData *skyShaderProgram,
                                    const IWGShaderProgramData *textShaderProgram,
                                    GLuint textureHandlerId,
                                    GLvoid* fontMapTextureData)
{
    // Save programs for later use
    startMenuController->mainShaderProgram = mainShaderProgram;
    startMenuController->skyShaderProgram = skyShaderProgram;
    startMenuController->textShaderProgram = textShaderProgram;
    startMenuController->fontMapTextureData = fontMapTextureData;
    startMenuController->textureHandlerId = textureHandlerId;
    
    // Text buffer
    glUseProgram(textShaderProgram->programID);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        startMenuController->textMultiBuffer.nVertices[i] = startMenuController->textDataBufferSize / 9;
        
        IWGRingBufferBind(&startMenuController->textMultiBuffer, i);
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontMapTextureData);
        
        glBufferData(GL_ARRAY_BUFFER,
                     startMenuController->textDataBufferSize * sizeof(GLfloat),
                     startMenuController->textDataBufferStart,
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
    
    // Menu
    IWUIMenuControllerFillVBO(&startMenuController->menuController,
                              textShaderProgram->vertexSlot, textShaderProgram->colorSlot,
                              textShaderProgram->textureOffsetSlot,
                              textureHandlerId, fontMapTextureData);
    
    // Cubes
    IWCubeControllerSetupVBOs(&startMenuController->cubeController,
                              mainShaderProgram);
    
    // Sky box
    IWGSkyBoxControllerFillVBO(&startMenuController->skyBoxController,
                               skyShaderProgram);
    
    return;
}

void IWStartMenuControllerUpdate(IWStartMenuControllerData *startMenuController,
                                 IWPoint2D touchPoint,
                                 bool *isTouched,
                                 float timeSinceLastUpdate)
{
    // Update menu
    IWUIMenuControllerUpdate(&startMenuController->menuController,
                             isTouched,
                             touchPoint,
                             timeSinceLastUpdate);
    
    // Update skybox
    IWGSkyBoxControllerUpdate(&startMenuController->skyBoxController, timeSinceLastUpdate,
                              &startMenuController->player, true);
    
    
    return;
}

void IWStartMenuControllerRender(IWStartMenuControllerData *startMenuController,
                                 const IWGShaderProgramData *mainShaderProgram,
                                 const IWGShaderProgramData *skyShaderProgram,
                                 const IWGShaderProgramData *textShaderProgram)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(mainShaderProgram->programID);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    IWCubeControllerRender(&startMenuController->cubeController, mainShaderProgram->programID);
    
    glDisable(GL_CULL_FACE);
    
    glUseProgram(skyShaderProgram->programID);
    
    IWGSkyBoxControllerRender(&startMenuController->skyBoxController);
    
    glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    
    glUseProgram(textShaderProgram->programID);
    
    IWUIMenuControllerRender(&startMenuController->menuController);
    
    IWGRingBufferBindCurrentDrawBuffer(&startMenuController->textMultiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0,
                 startMenuController->textMultiBuffer.nVertices[startMenuController->textMultiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGRingBufferSwitchBuffer(&startMenuController->textMultiBuffer);
    
    glDisable(GL_BLEND);
}

void IWStartMenuControllerPurgeData(IWStartMenuControllerData *startMenuController)
{
    IWCubeControllerPurgeData(&startMenuController->cubeController);
    IWGSkyBoxControllerPurgeData(&startMenuController->skyBoxController);
    IWUIMenuControllerPurgeData(&startMenuController->menuController);
    IWGRingBufferDealloc(&startMenuController->textMultiBuffer);
    free(startMenuController->textDataBufferStart);
    startMenuController->textDataBufferStart = NULL;
}