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

IWUserInterfaceControllerData IWUserInterfaceControllerMake(float screenAspectRatio,
                                                            IWUSERINTERFACE_ELEMENT visibleElements,
                                                            IWGFontMapData *fontMap)
{
    IWUserInterfaceControllerData userInterfaceController = {
        visibleElements,
        0, NULL, IWGRingBufferGen(),
        0, NULL, IWGRingBufferGen(),
        0, NULL, IWGRingBufferGen()
    };


    //
    // Setup text buffer
    //
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_SCORE)
        userInterfaceController.textDataBufferSize += 1 * 10 * 6 * 9;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER)
        userInterfaceController.textDataBufferSize += 3 * 10 * 6 * 9;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_FUEL_STATUS_BAR)
        userInterfaceController.textDataBufferSize += 1 * 15 * 6 * 9;

    userInterfaceController.textDataBufferStart = (GLfloat*)malloc(userInterfaceController.textDataBufferSize * sizeof(GLfloat));
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
        userInterfaceController.cubeStatusTextField1
            = IWGTextFieldMake(cubeCounterAnchorPosition,
                               IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                               2, 10,
                               1. / screenAspectRatio,
                               "",
                               0.1, -0.01,
                               IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                               IWVector4Make(1.0, 1.0, 1.0, 0.5),
                               fontMap,
                               userInterfaceController.textDataBufferStart
                               + bufferOffset);
        bufferOffset += userInterfaceController.cubeStatusTextField1.triangleBufferData.size;
        userInterfaceController.cubeStatusTextField2
        = IWGTextFieldMake(IWVector2Make(cubeCounterAnchorPosition.x,
                                         cubeCounterAnchorPosition.y - 2.0 * cubeCounterLineHeight),
                           IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                           1, 10,
                           1. / screenAspectRatio,
                           "",
                           0.1, -0.01,
                           IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                           IWVector4Make(1.0, 1.0, 1.0, 0.0),
                           fontMap,
                           userInterfaceController.textDataBufferStart
                           + bufferOffset);
        userInterfaceController.poolCubesColorTransition
            = IWVector4TransitionMake(IWUI_COLOR_WHITE(0.3),
                                      IWUI_COLOR_WHITE(0.7),
                                      IWUI_COLOR_WHITE(0.3),
                                      0.5, 0.0, false, false);
        bufferOffset += userInterfaceController.cubeStatusTextField2.triangleBufferData.size;
    }
    
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_FUEL_STATUS_BAR) {
        userInterfaceController.fuelStatusTextField
        = IWGTextFieldMake(IWVector2Make(-0.98, 0.95 * 2.0 - 1. - 0.01),
                           IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                           1, 15,
                           1. / screenAspectRatio,
                           "",
                           0.12, 0.0,
                           IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                           IWVector4Make(1.0, 1.0, 1.0, 0.7),
                           fontMap,
                           userInterfaceController.textDataBufferStart
                           + bufferOffset);
        userInterfaceController.fuelStatusTextField.triangleBufferData.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.fuelStatusTextField.triangleBufferData.size;
    }
    //
    // Setup triangle buffer
    //

    if (visibleElements & IWUSERINTERFACE_ELEMENT_FUEL_STATUS_BAR)
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

    userInterfaceController.triangleDataBufferStart = (GLfloat*)malloc(userInterfaceController.triangleDataBufferSize * sizeof(GLfloat));
    bufferOffset = 0;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_FUEL_STATUS_BAR) {
        float fuelBarStates[] = {1.0, 1.0, 1.0};
        IWVector4 fuelBarColors[] = {IWUI_COLOR_WHITE(0.8), IWUI_COLOR_WHITE(0.4), IWUI_COLOR_WHITE(0.2)};
        userInterfaceController.fuelStateBar = IWUIStateBarMake(3, fuelBarStates, fuelBarColors,
                                                                IWRectangleMake(0.01, 0.95, 0.4, 0.99),
                                                                IWUI_ORIENTATION_HORIZONTAL,
                                                                IWUI_DIRECTION_NORMAL);
        userInterfaceController.fuelStateBar.triangleBufferData.startCPU = userInterfaceController.triangleDataBufferStart + bufferOffset;
        IWUIStateBarToTriangles(&userInterfaceController.fuelStateBar);
        userInterfaceController.fuelStateBar.triangleBufferData.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.fuelStateBar.triangleBufferData.size;
        userInterfaceController.fuelColorTransitionOverdrive
            = IWVector4TransitionMake(IWUI_COLOR_WHITE(0.5),
                                      IWUI_COLOR_WHITE(0.8),
                                      IWUI_COLOR_WHITE(0.5),
                                      0.33, 0.0, false, false);
        userInterfaceController.fuelColorTransitionWarning
            = IWVector4TransitionMake(IWUI_COLOR_RED(0.2),
                                      IWUI_COLOR_RED(0.8),
                                      IWUI_COLOR_RED(0.2),
                                      0.33, 0.0, false, false);
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
        bufferOffset += cubeSymbol.triangleBuffer.size;
        cubeCounterCubeSymbolRect.lowerLeft.y -= cubeCounterLineHeight;
        cubeCounterCubeSymbolRect.upperRight.y -= cubeCounterLineHeight;
        cubeSymbol = IWUIElementMakeCubeSymbol(cubeCounterCubeSymbolRect,
                                               0.5,
                                               IWUI_COLOR_RED(0.8),
                                               IWUI_COLOR_DARK_RED(0.8),
                                               userInterfaceController.triangleDataBufferStart
                                               + bufferOffset);
        bufferOffset += cubeSymbol.triangleBuffer.size;
    }
    
    //
    // Setup line buffer
    //
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_PAUSE_BUTTON)
         userInterfaceController.lineDataBufferSize += IWUIRectangleButtonLineBufferSize(&userInterfaceController.pauseButton);
    if (visibleElements & IWUSERINTERFACE_ELEMENT_HUD) {
        userInterfaceController.lineDataBufferSize += (41 + 11 + 4 * 21) * 2 * 7;
    }
    
    userInterfaceController.lineDataBufferStart = (GLfloat*)malloc(userInterfaceController.lineDataBufferSize * sizeof(GLfloat));
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
        bufferOffset += uiCentralCircle.lineBuffer.size;
        
        IWUIElementData uiCentralCircle2 = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.001,//0.01,//0.1,//0.005,
                                                                 IWVector4Make(1.0, 1.0, 1.0, 0.4), screenAspectRatio, 11,
                                                                 userInterfaceController.lineDataBufferStart
                                                                 + bufferOffset);
        bufferOffset += uiCentralCircle2.lineBuffer.size;
        
        float archRadius = 0.065;
        
        userInterfaceController.upperArch = IWUIElementMakeArch(IWVector2Make(0.5, 0.5), archRadius,
                                                                IWVector4Make(1.0, 1.0, 1.0, 0.4),
                                                                screenAspectRatio,
                                                                21,
                                                                userInterfaceController.lineDataBufferStart
                                                                + bufferOffset,
                                                                2. * M_PI * (1.0 - 0.125),
                                                                2. * M_PI * (1.0 + 0.125));
        userInterfaceController.upperArch.lineBuffer.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.upperArch.lineBuffer.size;
        
        userInterfaceController.lowerArch = IWUIElementMakeArch(IWVector2Make(0.5, 0.5), archRadius,
                                                                IWVector4Make(1.0, 1.0, 1.0, 0.4),
                                                                screenAspectRatio,
                                                                21,
                                                                userInterfaceController.lineDataBufferStart
                                                                + bufferOffset,
                                                                2. * M_PI * (0.5 - 0.125),
                                                                2. * M_PI * (0.5 + 0.125));
        userInterfaceController.lowerArch.lineBuffer.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.lowerArch.lineBuffer.size;
        
        userInterfaceController.rightArch = IWUIElementMakeArch(IWVector2Make(0.5, 0.5), archRadius,
                                                                IWVector4Make(1.0, 1.0, 1.0, 0.4),
                                                                screenAspectRatio,
                                                                21,
                                                                userInterfaceController.lineDataBufferStart
                                                                + bufferOffset,
                                                                2. * M_PI * (0.25 - 0.125),
                                                                2. * M_PI * (0.25 + 0.125));
        userInterfaceController.rightArch.lineBuffer.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.rightArch.lineBuffer.size;
        
        userInterfaceController.leftArch = IWUIElementMakeArch(IWVector2Make(0.5, 0.5), archRadius,
                                                                IWVector4Make(1.0, 1.0, 1.0, 0.4),
                                                                screenAspectRatio,
                                                                21,
                                                                userInterfaceController.lineDataBufferStart
                                                                + bufferOffset,
                                                                2. * M_PI * (0.75 - 0.125),
                                                                2. * M_PI * (0.75 + 0.125));
        userInterfaceController.leftArch.lineBuffer.bufferOffsetGPU = bufferOffset;
        bufferOffset += userInterfaceController.leftArch.lineBuffer.size;
        
    }
    
#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    
    return userInterfaceController;
}

void IWUserInterfaceControllerSetupVBOs(IWUserInterfaceControllerData *userInterfaceController,
                                        const IWGShaderProgramData *uiShaderProgram,
                                        const IWGShaderProgramData *textShaderProgram,
                                        GLuint textureHandlerId)
{
    // Fill buffers
    
    // Text buffer
    glUseProgram(textShaderProgram->programID);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        userInterfaceController->textMultiBuffer.nVertices[i] = userInterfaceController->textDataBufferSize / 9;
        
        IWGRingBufferBind(&userInterfaceController->textMultiBuffer, i);
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, gdFontMapTextureData);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontMapTextureData);
//        //glGenerateMipmap(GL_TEXTURE_2D);
        
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
    
#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    
    //
    // UI Buffer
    //
    
    glUseProgram(uiShaderProgram->programID);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        userInterfaceController->triangleMultiBuffer.nVertices[i] = userInterfaceController->triangleDataBufferSize / 7;
        
        IWGRingBufferBind(&userInterfaceController->triangleMultiBuffer, i);
        
        glBufferData(GL_ARRAY_BUFFER,
                     userInterfaceController->triangleDataBufferSize * sizeof(GLfloat),
                     userInterfaceController->triangleDataBufferStart,
                     GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(uiShaderProgram->vertexSlot);
        glVertexAttribPointer(uiShaderProgram->vertexSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(uiShaderProgram->colorSlot);
        glVertexAttribPointer(uiShaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    }
    
#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        userInterfaceController->lineMultiBuffer.nVertices[i] = userInterfaceController->lineDataBufferSize / 7;
        
        IWGRingBufferBind(&userInterfaceController->lineMultiBuffer, i);
        
        glBufferData(GL_ARRAY_BUFFER,
                     userInterfaceController->lineDataBufferSize * sizeof(GLfloat),
                     userInterfaceController->lineDataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(uiShaderProgram->vertexSlot);
        glVertexAttribPointer(uiShaderProgram->vertexSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(uiShaderProgram->colorSlot);
        glVertexAttribPointer(uiShaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    }
    
#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    
    return;
}

void IWUserInterfaceControllerUpdate(IWUserInterfaceControllerData *userInterfaceController,
                                     const IWScoreCounterData *scoreCounter,
                                     const IWCubeStatusData *cubeStatus,
                                     const IWFuel *fuel,
                                     const IWPlayerData *player,
                                     const IWControllerData *controller,
                                     float timeSinceLastUpdate)
{
    if (userInterfaceController->visibleElements & IWUSERINTERFACE_ELEMENT_SCORE) {
        // Update score counter
        char s[10];
        sprintf(s, "%u", scoreCounter->scoreInt);
        IWGTextFieldSetText(&userInterfaceController->scoreTextField, s);
        IWGRingBufferSubData(&userInterfaceController->textMultiBuffer,
                              0,
                              userInterfaceController->scoreTextField.triangleBufferData.size * sizeof(GLfloat),
                              userInterfaceController->scoreTextField.triangleBufferData.startCPU,
                              true);
    }
    
    if (userInterfaceController->visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER) {
        // Update cube status display
        char sTmp[30];
        sprintf(sTmp, "%u\n%u", cubeStatus->nGridCubes, cubeStatus->nBridgeCubes);
        IWGTextFieldSetText(&userInterfaceController->cubeStatusTextField1, sTmp);
        IWGRingBufferSubData(&userInterfaceController->textMultiBuffer,
                              userInterfaceController->scoreTextField.triangleBufferData.size * sizeof(GLfloat),
                              userInterfaceController->cubeStatusTextField1.triangleBufferData.size * sizeof(GLfloat),
                              userInterfaceController->cubeStatusTextField1.triangleBufferData.startCPU,
                              true);
        unsigned int nPoolCubes = cubeStatus->nPoolCubes;
        sprintf(sTmp, "%u", nPoolCubes);
        if (nPoolCubes < 10) {
            userInterfaceController->cubeStatusTextField2.color.w = 0.5 * (float)nPoolCubes / 10.0;
        } else {
            if(IWVector4TransitionUpdate(&userInterfaceController->poolCubesColorTransition, timeSinceLastUpdate)) {
                IWVector4TransitionReverseAndStart(&userInterfaceController->poolCubesColorTransition);
            }
            userInterfaceController->cubeStatusTextField2.color
                = userInterfaceController->poolCubesColorTransition.currentVector;
        }
        IWGTextFieldSetText(&userInterfaceController->cubeStatusTextField2, sTmp);
        IWGRingBufferSubData(&userInterfaceController->textMultiBuffer,
                              (userInterfaceController->scoreTextField.triangleBufferData.size
                               + userInterfaceController->cubeStatusTextField1.triangleBufferData.size) * sizeof(GLfloat),
                              userInterfaceController->cubeStatusTextField2.triangleBufferData.size * sizeof(GLfloat),
                              userInterfaceController->cubeStatusTextField2.triangleBufferData.startCPU,
                              true);
    }
    
    // Update fuel status bar
    if (userInterfaceController->visibleElements & IWUSERINTERFACE_ELEMENT_FUEL_STATUS_BAR) {
        
        if (player->overdrive) {
            
            if(IWVector4TransitionUpdate(&userInterfaceController->fuelColorTransitionOverdrive, timeSinceLastUpdate))
                IWVector4TransitionReverseAndStart(&userInterfaceController->fuelColorTransitionOverdrive);
            
            userInterfaceController->fuelStateBar.colors[0]
            = userInterfaceController->fuelColorTransitionOverdrive.currentVector;
            
            userInterfaceController->fuelStatusTextField.color
            = userInterfaceController->fuelColorTransitionOverdrive.currentVector;
            IWGTextFieldSetText(&userInterfaceController->fuelStatusTextField, "OVERDRIVE");
            
        } else if (fuel->isWarning) {
            
            if(IWVector4TransitionUpdate(&userInterfaceController->fuelColorTransitionWarning, timeSinceLastUpdate))
                IWVector4TransitionReverseAndStart(&userInterfaceController->fuelColorTransitionWarning);
            userInterfaceController->fuelStateBar.colors[0]
            = userInterfaceController->fuelColorTransitionWarning.currentVector;
            
            userInterfaceController->fuelStatusTextField.color
            = userInterfaceController->fuelColorTransitionWarning.currentVector;
            
            IWGTextFieldSetText(&userInterfaceController->fuelStatusTextField, "FUEL WARNING");
            
        } else  {
            userInterfaceController->fuelStateBar.colors[0] = fuel->currentColor;
            IWGTextFieldSetText(&userInterfaceController->fuelStatusTextField, "");
        }
        
        // Fuel vertex update
        IWFuelToStateBar(fuel, &userInterfaceController->fuelStateBar);
        IWUIStateBarToTriangles(&userInterfaceController->fuelStateBar);
        
        IWGRingBufferSubData(&userInterfaceController->triangleMultiBuffer,
                             0,
                             userInterfaceController->fuelStateBar.triangleBufferData.size * sizeof(GLfloat),
                             userInterfaceController->fuelStateBar.triangleBufferData.startCPU,
                             true);
        
        // Fuel text update
        IWGRingBufferSubData(&userInterfaceController->textMultiBuffer,
                             (userInterfaceController->fuelStatusTextField.triangleBufferData.bufferOffsetGPU) * sizeof(GLfloat),
                             userInterfaceController->fuelStatusTextField.triangleBufferData.size * sizeof(GLfloat),
                             userInterfaceController->fuelStatusTextField.triangleBufferData.startCPU,
                             true);
    }
    
    if ((userInterfaceController->visibleElements & IWUSERINTERFACE_ELEMENT_HUD)) {
        float controlIndicatorAlphaMax = 0.5;
        IWGPrimitiveBufferDataUpdateColor(&userInterfaceController->upperArch.lineBuffer,
                                          IWVector4Make(1.0, 1.0, 1.0,
                                                        controller->rotationSpeed.x > 0.0
                                                        ? controller->rotationSpeed.x * controlIndicatorAlphaMax
                                                        : 0.0));
        IWGRingBufferSubDataForBufferObject(&userInterfaceController->lineMultiBuffer,
                                            &userInterfaceController->upperArch.lineBuffer,
                                            true);
        IWGPrimitiveBufferDataUpdateColor(&userInterfaceController->lowerArch.lineBuffer,
                                          IWVector4Make(1.0, 1.0, 1.0,
                                                        controller->rotationSpeed.x < 0.0
                                                        ? fabs(controller->rotationSpeed.x) * controlIndicatorAlphaMax
                                                        : 0.0));
        IWGRingBufferSubDataForBufferObject(&userInterfaceController->lineMultiBuffer,
                                            &userInterfaceController->lowerArch.lineBuffer,
                                            true);
        IWGPrimitiveBufferDataUpdateColor(&userInterfaceController->leftArch.lineBuffer,
                                          IWVector4Make(1.0, 1.0, 1.0,
                                                        controller->rotationSpeed.y > 0.0
                                                        ? controller->rotationSpeed.y * controlIndicatorAlphaMax
                                                        : 0.0));
        IWGRingBufferSubDataForBufferObject(&userInterfaceController->lineMultiBuffer,
                                            &userInterfaceController->leftArch.lineBuffer,
                                            true);
        IWGPrimitiveBufferDataUpdateColor(&userInterfaceController->rightArch.lineBuffer,
                                          IWVector4Make(1.0, 1.0, 1.0,
                                                        controller->rotationSpeed.y < 0.0
                                                        ? fabs(controller->rotationSpeed.y) * controlIndicatorAlphaMax
                                                        : 0.0));
        IWGRingBufferSubDataForBufferObject(&userInterfaceController->lineMultiBuffer,
                                            &userInterfaceController->rightArch.lineBuffer,
                                            true);
        
    }

#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    return;
}

void IWUserInterfaceControllerRender(IWUserInterfaceControllerData *userInterfaceController,
                                     const IWGShaderProgramData *uiShaderProgram,
                                     const IWGShaderProgramData *textShaderProgram)
{
    glUseProgram(textShaderProgram->programID);
    
    IWGRingBufferBindCurrentDrawBuffer(&userInterfaceController->textMultiBuffer);

#ifndef IW_USE_GLVAO
    if (textShaderProgram) {
        IWGShaderProgramEnableVertexAtrribArrays(textShaderProgram, 9);
    } else {
        printf("ERROR IWUserInterfaceControllerRender: textShaderProgram == NULL\n");
        return;
    }
#endif
    
    glDrawArrays(GL_TRIANGLES, 0, userInterfaceController->textMultiBuffer.nVertices[userInterfaceController->textMultiBuffer.currentDrawBuffer]);
    
#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    
    IWGRingBufferSwitchBuffer(&userInterfaceController->textMultiBuffer);
    
    glUseProgram(uiShaderProgram->programID);
    
    IWGRingBufferBindCurrentDrawBuffer(&userInterfaceController->triangleMultiBuffer);

#ifndef IW_USE_GLVAO
    if (uiShaderProgram) {
        IWGShaderProgramEnableVertexAtrribArrays(uiShaderProgram, 7);
    } else {
        printf("ERROR IWUserInterfaceControllerRender: uiShaderProgram == NULL\n");
        return;
    }
#endif
    
    glDrawArrays(GL_TRIANGLES, 0, userInterfaceController->triangleMultiBuffer.nVertices[userInterfaceController->triangleMultiBuffer.currentDrawBuffer]);
    
    IWGRingBufferSwitchBuffer(&userInterfaceController->triangleMultiBuffer);
    
    IWGRingBufferBindCurrentDrawBuffer(&userInterfaceController->lineMultiBuffer);

#ifndef IW_USE_GLVAO
    if (uiShaderProgram) {
        IWGShaderProgramEnableVertexAtrribArrays(uiShaderProgram, 7);
    } else {
        printf("ERROR IWUserInterfaceControllerRender: uiShaderProgram == NULL\n");
        return;
    }
#endif
    
    glDrawArrays(GL_LINES, 0, userInterfaceController->lineMultiBuffer.nVertices[userInterfaceController->lineMultiBuffer.currentDrawBuffer]);

#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    
    IWGRingBufferSwitchBuffer(&userInterfaceController->lineMultiBuffer);

    return;
}

void IWUserInterfacePurgeData(IWUserInterfaceControllerData *userInterfaceController)
{
    IWGRingBufferDealloc(&userInterfaceController->textMultiBuffer);
    IWGRingBufferDealloc(&userInterfaceController->triangleMultiBuffer);
    IWGRingBufferDealloc(&userInterfaceController->lineMultiBuffer);
    IWUIStateBarDeallocData(&userInterfaceController->fuelStateBar);
    free(userInterfaceController->textDataBufferStart);
    free(userInterfaceController->triangleDataBufferStart);
    free(userInterfaceController->lineDataBufferStart);
    return;
}
