//
//  IWUserInterfaceController.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/28/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUserInterfaceController.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IWUserInterface.h"
#include "IWGeometry.h"
#include "IWUIElement.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWUserInterfaceControllerData IWUserInterfaceControllerMake(float screenAspectRatio,
                                                            IWUSERINTERFACE_ELEMENT visibleElements,
                                                            IWGFontMapData *fontMap)
{
    IWUserInterfaceControllerData userInterfaceController = {
        visibleElements,
        0, NULL, IWGMultiBufferGen(),
        0, NULL, IWGMultiBufferGen(),
        0, NULL, IWGMultiBufferGen()
    };

    
    //gdInGameTextTriangleBufferStartCPU = malloc((1 * 10 + 3 * 10) * 6 * 9 * sizeof(GLfloat));

    //
    // Setup text buffer
    //
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_SCORE)
        userInterfaceController.textDataBufferSize += 1 * 10 * 6 * 9;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER)
        userInterfaceController.textDataBufferSize += 3 * 10 * 6 * 9;
    
    userInterfaceController.textDataBufferStart = malloc(userInterfaceController.textDataBufferSize * sizeof(GLfloat));
    GLuint bufferOffset = 0;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_SCORE) {
        userInterfaceController.scoreTextField
            = IWGTextFieldMake(IWVector2Make(0.94, 1.0),
                               IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                               1, 10,
                               1. / screenAspectRatio,
                               "",
                               0.4, -0.04,
                               IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                               IWVector4Make(1.0, 1.0, 1.0, 0.7),
                               fontMap,
                               userInterfaceController.textDataBufferStart);
        bufferOffset += userInterfaceController.scoreTextField.triangleBufferData.size;
    }
    
    IWPoint2D cubeCounterAnchorPosition = IWVector2Make(0.91, 0.65);
    float cubeCounterLineHeight = 0.09;
    if (visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER) {
        userInterfaceController.cubeStatusTextField
            = IWGTextFieldMake(cubeCounterAnchorPosition,
                               IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                               3, 10,
                               1. / screenAspectRatio,
                               "",
                               0.1, -0.01,
                               IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                               IWVector4Make(1.0, 1.0, 1.0, 0.5),
                               fontMap,
                               userInterfaceController.textDataBufferStart
                               + bufferOffset);
    }
    
    //
    // Setup triangle buffer
    //

    if (visibleElements & IWUSERINTERFACE_ELEMENT_ENERGY_BAR)
        userInterfaceController.triangleDataBufferSize += 6 * 3 * 7;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_PAUSE_BUTTON) {
        userInterfaceController.pauseButton = IWUIRectangleButtonMake(0.0, -0.001,
                                                                      IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
                                                                      0.18, 0.19,
                                                                      IWUI_COLOR_WHITE(0.5),
                                                                      IWUI_COLOR_WHITE(0.25),
                                                                      IWUI_COLOR_WHITE(0.5),
                                                                      (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT),
                                                                      0.035, 1. / screenAspectRatio);
        userInterfaceController.triangleDataBufferSize
            += IWUIRectangleButtonTriangleBufferSize(&userInterfaceController.pauseButton);
    }
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER)
        userInterfaceController.triangleDataBufferSize += 6 * 3 * 7 * 2;

    userInterfaceController.triangleDataBufferStart = malloc(userInterfaceController.triangleDataBufferSize * sizeof(GLfloat));
    bufferOffset = 0;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_ENERGY_BAR) {
        float fuelBarStates[] = {1.0, 1.0, 1.0};
        IWVector4 fuelBarColors[] = {IWUI_COLOR_WHITE(0.8), IWUI_COLOR_WHITE(0.4), IWUI_COLOR_WHITE(0.2)};
        userInterfaceController.fuelStateBar = IWUIStateBarMake(3, fuelBarStates, fuelBarColors,
                                                                IWRectangleMake(0.01, 0.95, 0.4, 0.99),
                                                                IWUI_ORIENTATION_HORIZONTAL,
                                                                IWUI_DIRECTION_NORMAL);
        userInterfaceController.fuelStateBar.triangleBufferData.bufferStartCPU = userInterfaceController.triangleDataBufferStart + bufferOffset;
        IWUIStateBarToTriangles(&userInterfaceController.fuelStateBar);
        userInterfaceController.fuelStateBar.triangleBufferData.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.fuelStateBar.triangleBufferData.size;
    }

    if (visibleElements & IWUSERINTERFACE_ELEMENT_PAUSE_BUTTON) {
        IWUIRectangleButtonToTriangleBuffer(&userInterfaceController.pauseButton, userInterfaceController.triangleDataBufferStart + bufferOffset);
        userInterfaceController.pauseButton.triangleBuffer.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.pauseButton.triangleBuffer.size;
    }
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER) {
        IWRectangle cubeCounterCubeSymbolRect =
        IWRectangleMake(cubeCounterAnchorPosition.x + 0.02,
                        cubeCounterAnchorPosition.y - cubeCounterLineHeight * 0.85 - 0.005,
                        cubeCounterAnchorPosition.x + 0.02 + cubeCounterLineHeight * 0.5 *screenAspectRatio,
                        cubeCounterAnchorPosition.y - cubeCounterLineHeight * 0.15 - 0.005);
        
        IWUIElementData cubeSymbol = IWUIElementMakeCubeSymbol(cubeCounterCubeSymbolRect,
                                                               0.5,
                                                               IWVector4Make(0.5, 0.5, 0.5, 0.8),
                                                               IWVector4Make(0.3, 0.3, 0.3, 0.8),
                                                               userInterfaceController.triangleDataBufferStart
                                                               + bufferOffset);
        bufferOffset += cubeSymbol.triangleBufferSize;
        cubeCounterCubeSymbolRect.lowerLeft.y -= cubeCounterLineHeight;
        cubeCounterCubeSymbolRect.upperRight.y -= cubeCounterLineHeight;
        cubeSymbol = IWUIElementMakeCubeSymbol(cubeCounterCubeSymbolRect,
                                               0.5,
                                               IWUI_COLOR_RED(0.8),
                                               IWUI_COLOR_DARK_RED(0.8),
                                               userInterfaceController.triangleDataBufferStart
                                               + bufferOffset);
        bufferOffset += cubeSymbol.triangleBufferSize;
    }
    
    //
    // Setup line buffer
    //
     if (visibleElements & IWUSERINTERFACE_ELEMENT_PAUSE_BUTTON)
         userInterfaceController.lineDataBufferSize += IWUIRectangleButtonLineBufferSize(&userInterfaceController.pauseButton);
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_HUD) {
        userInterfaceController.lineDataBufferSize += 2 * 52 * 7;
    }
    
    userInterfaceController.lineDataBufferStart = malloc(userInterfaceController.lineDataBufferSize * sizeof(GLfloat));
    bufferOffset = 0;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_PAUSE_BUTTON) {
        bufferOffset += IWUIRectangleButtonToLineBuffer(&userInterfaceController.pauseButton,
                                                        userInterfaceController.lineDataBufferStart);
    }

    if (visibleElements & IWUSERINTERFACE_ELEMENT_HUD) {
        IWUIElementData uiCentralCircle = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.06,//0.05,//0.3,//0.03,
                                                                IWVector4Make(1.0, 1.0, 1.0, 0.3), screenAspectRatio, 41,
                                                                userInterfaceController.lineDataBufferStart
                                                                + bufferOffset);
        bufferOffset += uiCentralCircle.lineBufferSize;
        
        IWUIElementData uiCentralCircle2 = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.001,//0.01,//0.1,//0.005,
                                                                 IWVector4Make(1.0, 1.0, 1.0, 0.4), screenAspectRatio, 11,
                                                                 userInterfaceController.lineDataBufferStart
                                                                 + bufferOffset);
        bufferOffset += uiCentralCircle2.lineBufferSize;
    }
    
    glBindVertexArrayOES(0);
    
    return userInterfaceController;
}

void IWUserInterfaceControllerSetupVBOs(IWUserInterfaceControllerData *userInterfaceController,
                                        const IWGShaderProgramData *uiShaderProgram,
                                        const IWGShaderProgramData *textShaderProgram,
                                        GLuint textureHandlerId,
                                        GLvoid* fontMapTextureData)
{
    // Fill buffers
    
    // Text buffer
    glUseProgram(textShaderProgram->programID);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        userInterfaceController->textMultiBuffer.nVertices[i] = userInterfaceController->textDataBufferSize / 9;
        
        IWGMultiBufferBind(&userInterfaceController->textMultiBuffer, i);
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, gdFontMapTextureData);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontMapTextureData);
        //glGenerateMipmap(GL_TEXTURE_2D);
        
        glBufferData(GL_ARRAY_BUFFER,
                     userInterfaceController->textDataBufferSize * sizeof(GLfloat),
                     userInterfaceController->textDataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(textShaderProgram->vertexSlot);
        glVertexAttribPointer(textShaderProgram->vertexSlot, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(textShaderProgram->colorSlot);
        glVertexAttribPointer(textShaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textShaderProgram->textureOffsetSlot);
        glVertexAttribPointer(textShaderProgram->textureOffsetSlot, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    
    glBindVertexArrayOES(0);
    
    //
    // UI Buffer
    //
    
    glUseProgram(uiShaderProgram->programID);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        userInterfaceController->triangleMultiBuffer.nVertices[i] = userInterfaceController->triangleDataBufferSize / 7;
        
        IWGMultiBufferBind(&userInterfaceController->triangleMultiBuffer, i);
        
        glBufferData(GL_ARRAY_BUFFER,
                     userInterfaceController->triangleDataBufferSize * sizeof(GLfloat),
                     userInterfaceController->triangleDataBufferStart,
                     GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(uiShaderProgram->vertexSlot);
        glVertexAttribPointer(uiShaderProgram->vertexSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(uiShaderProgram->colorSlot);
        glVertexAttribPointer(uiShaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    }
    
    glBindVertexArrayOES(0);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        userInterfaceController->lineMultiBuffer.nVertices[i] = userInterfaceController->lineDataBufferSize / 7;
        
        IWGMultiBufferBind(&userInterfaceController->lineMultiBuffer, i);
        
        glBufferData(GL_ARRAY_BUFFER,
                     userInterfaceController->lineDataBufferSize * sizeof(GLfloat),
                     userInterfaceController->lineDataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(uiShaderProgram->vertexSlot);
        glVertexAttribPointer(uiShaderProgram->vertexSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(uiShaderProgram->colorSlot);
        glVertexAttribPointer(uiShaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    }
    
    glBindVertexArrayOES(0);
    
    return;
}

void IWUserInterfaceControllerUpdate(IWUserInterfaceControllerData *userInterfaceController,
                                     const IWScoreCounterData *scoreCounter,
                                     const IWGameStatusData *gameStatus,
                                     float timeSinceLastUpdate)
{
    if (userInterfaceController->visibleElements & IWUSERINTERFACE_ELEMENT_SCORE) {
        // Update score counter
        char s[10];
        sprintf(s, "%u", scoreCounter->scoreInt);
        IWGTextFieldSetText(&userInterfaceController->scoreTextField, s);
        IWGMultiBufferSubData(&userInterfaceController->textMultiBuffer,
                              0,
                              userInterfaceController->scoreTextField.triangleBufferData.size * sizeof(GLfloat),
                              userInterfaceController->scoreTextField.triangleBufferData.startCPU,
                              true);
    }
    
    if (userInterfaceController->visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER) {
        // Update cube status display
        char sTmp[30];
        sprintf(sTmp, "%u\n%u\n%u", gameStatus->nGridCubes, gameStatus->nBridgeCubes, gameStatus->nPoolCubes);
        IWGTextFieldSetText(&userInterfaceController->cubeStatusTextField, sTmp);
        IWGMultiBufferSubData(&userInterfaceController->textMultiBuffer,
                              userInterfaceController->scoreTextField.triangleBufferData.size * sizeof(GLfloat),
                              userInterfaceController->cubeStatusTextField.triangleBufferData.size * sizeof(GLfloat),
                              userInterfaceController->cubeStatusTextField.triangleBufferData.startCPU,
                              true);
    }
    return;
}

void IWUserInterfaceControllerRender(IWUserInterfaceControllerData *userInterfaceController,
                                     GLuint textProgramId,
                                     GLuint uiProgramId)
{
    glUseProgram(textProgramId);
    
    IWGMultiBufferBindCurrentDrawBuffer(&userInterfaceController->textMultiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, userInterfaceController->textMultiBuffer.nVertices[userInterfaceController->textMultiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGMultiBufferSwitchBuffer(&userInterfaceController->textMultiBuffer);
    
    glUseProgram(uiProgramId);
    
    IWGMultiBufferBindCurrentDrawBuffer(&userInterfaceController->triangleMultiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, userInterfaceController->triangleMultiBuffer.nVertices[userInterfaceController->triangleMultiBuffer.currentDrawBuffer]);
    
    IWGMultiBufferSwitchBuffer(&userInterfaceController->triangleMultiBuffer);
    
    IWGMultiBufferBindCurrentDrawBuffer(&userInterfaceController->lineMultiBuffer);
    
    glDrawArrays(GL_LINES, 0, userInterfaceController->lineMultiBuffer.nVertices[userInterfaceController->lineMultiBuffer.currentDrawBuffer]);

    glBindVertexArrayOES(0);
    
    IWGMultiBufferSwitchBuffer(&userInterfaceController->lineMultiBuffer);

    return;
}

void IWUserInterfacePurgeData(IWUserInterfaceControllerData *userInterfaceController)
{
    IWGMultiBufferDealloc(&userInterfaceController->textMultiBuffer);
    IWGMultiBufferDealloc(&userInterfaceController->triangleMultiBuffer);
    IWGMultiBufferDealloc(&userInterfaceController->lineMultiBuffer);
    IWUIStateBarDeallocData(&userInterfaceController->fuelStateBar);
    free(userInterfaceController->textDataBufferStart);
    free(userInterfaceController->triangleDataBufferStart);
    free(userInterfaceController->lineDataBufferStart);
    return;
}
