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
    
    // Setup text buffer
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_SCORE)
        userInterfaceController.textDataBufferSize += 1 * 10 * 6 * 9;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER)
        userInterfaceController.textDataBufferSize += 3 * 10 * 6 * 9;
    
    userInterfaceController.textDataBufferStart = malloc(userInterfaceController.textDataBufferSize * sizeof(GLfloat));
    GLuint bufferOffset = 0;
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_SCORE) {
        userInterfaceController.scoreTextField
            = IWGTextFieldMake(IWVector2Make(0.95, 1.0),
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
    
    if (visibleElements & IWUSERINTERFACE_ELEMENT_CUBE_COUNTER) {
        userInterfaceController.cubeStatusTextField
            = IWGTextFieldMake(IWVector2Make(0.93, 0.62),
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
    
//    //
//    // Head up display data
//    //
//    
//    // Some buttons
//    gdRectangleButton = IWUIRectangleButtonMake(0.0, -0.001,
//                                                IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
//                                                0.18, 0.19,
//                                                IWUI_COLOR_WHITE(0.5),
//                                                IWUI_COLOR_WHITE(0.25),
//                                                IWUI_COLOR_WHITE(0.5),
//                                                (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT),
//                                                0.035, 1. / aspect);
//    gdRectangleButton2 = IWUIRectangleButtonMake(0.82, -0.001,
//                                                 IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
//                                                 0.18, 0.19,
//                                                 IWUI_COLOR_PURPLE(0.3), IWUI_COLOR_WHITE(0.25),
//                                                 IWUI_COLOR_WHITE(0.5),
//                                                 (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
//                                                 0.035, 1. / aspect);
//    
//    gdUINTriangleVertices = ((IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton)
//                              + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton2)) / 7
//                             + 6 * 3// 4 for reverse, 3 for normal
//                             //+ 6 * 4// cube counter
//                             );
//    
//    size_t mypos_size2 = gdUINTriangleVertices * 7 * sizeof(GLfloat);
//    gdInGameUITriangleBufferStartCPU = malloc(mypos_size2);
//    
//    gdRectangleButton.triangleBuffer.bufferOffsetGPU = 0;
//    size_t offset = IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton, gdInGameUITriangleBufferStartCPU);
//    gdRectangleButton2.triangleBuffer.bufferOffsetGPU = offset;
//    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton2, gdInGameUITriangleBufferStartCPU + offset);
//    
////    gdFuel.stateBar.triangleBufferData.bufferStartCPU = gdInGameUITriangleBufferStartCPU + offset;
////    gdFuel.stateBar.triangleBufferData.bufferOffsetGPU = offset;
////    gdFuel.stateBar.triangleBufferData.size = IWFuelToTriangleBuffer(&gdFuel, gdInGameUITriangleBufferStartCPU + offset);
////    offset += gdFuel.stateBar.triangleBufferData.size;
//    
//    glUseProgram(gdUIShaderProgram.programID);
//    
//    positionSlot = glGetAttribLocation(gdUIShaderProgram.programID, "Vertex");
//    colorSlot = glGetAttribLocation(gdUIShaderProgram.programID, "Color");
//    
//    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
//        IWGMultiBufferBind(&gdUITriangleDoubleBuffer, i);
//        glBufferData(GL_ARRAY_BUFFER, mypos_size2, gdInGameUITriangleBufferStartCPU, GL_DYNAMIC_DRAW);
//        
//        glEnableVertexAttribArray(positionSlot);
//        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
//        glEnableVertexAttribArray(colorSlot);
//        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
//    }
//    
//    glBindVertexArrayOES(0);
//    
//    glBindVertexArrayOES(gdUILineVertexArray);
//    
//    gdUINLineVertices = (IWUIRectangleButtonLineBufferSize(&gdRectangleButton)
//                         + IWUIRectangleButtonLineBufferSize(&gdRectangleButton2)
//                         + 2 * 52 * 7) / 7;
//    
//    mypos_size2 = gdUINLineVertices * 7 * sizeof(GLfloat);
//    gdInGameUILineBufferStartCPU = malloc(mypos_size2);
//    
//    gdRectangleButton.lineBuffer.bufferOffsetGPU = 0;
//    offset = IWUIRectangleButtonToLineBuffer(&gdRectangleButton, gdInGameUILineBufferStartCPU);
//    gdRectangleButton2.lineBuffer.bufferOffsetGPU = offset;
//    offset += IWUIRectangleButtonToLineBuffer(&gdRectangleButton2, gdInGameUILineBufferStartCPU + offset);
//    
//    IWUIElementData uiCentralCircle = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.06,//0.05,//0.3,//0.03,
//                                                            IWVector4Make(1.0, 1.0, 1.0, 0.3), aspect, 41,
//                                                            gdInGameUILineBufferStartCPU + offset);
//    offset += uiCentralCircle.lineBufferSize;
//    
//    IWUIElementData uiCentralCircle2 = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.001,//0.01,//0.1,//0.005,
//                                                             IWVector4Make(1.0, 1.0, 1.0, 0.4), aspect, 11,
//                                                             gdInGameUILineBufferStartCPU + offset);
//    offset += uiCentralCircle2.lineBufferSize;
//    
//    glBindBuffer(GL_ARRAY_BUFFER, gdUILineVertexBuffer);
//    glBufferData(GL_ARRAY_BUFFER, mypos_size2, gdInGameUILineBufferStartCPU, GL_DYNAMIC_DRAW);
//    
//    glEnableVertexAttribArray(positionSlot);
//    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
//    glEnableVertexAttribArray(colorSlot);
//    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
//    
//    glBindVertexArrayOES(0);
    
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
    
    return;
}

void IWUserInterfaceControllerUpdate(IWUserInterfaceControllerData *userInterfaceController,
                                     float timeSinceLastUpdate)
{
    return;
}

void IWUserInterfaceControllerRender(IWUserInterfaceControllerData *userInterfaceController)
{
    IWGMultiBufferBindCurrentDrawBuffer(&userInterfaceController->textMultiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, userInterfaceController->textMultiBuffer.nVertices[userInterfaceController->textMultiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGMultiBufferSwitchBuffer(&userInterfaceController->textMultiBuffer);

    return;
}

void IWUserInterfacePurgeData(IWUserInterfaceControllerData *userInterfaceController)
{
    IWGMultiBufferDealloc(&userInterfaceController->textMultiBuffer);
    IWGMultiBufferDealloc(&userInterfaceController->triangleMultiBuffer);
    IWGMultiBufferDealloc(&userInterfaceController->lineMultiBuffer);
    free(userInterfaceController->textDataBufferStart);
    return;
}
