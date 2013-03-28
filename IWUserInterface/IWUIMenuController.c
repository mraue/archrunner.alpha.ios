//
//  IWUIMenu.c
//  theBr1dge
//
//  Created by Martin Raue on 2/20/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIMenuController.h"

#include <stdio.h>
#include <stdlib.h>

IWUIMenuControllerData IWUIMenuControllerMake(IWUIMenuPresenterData presenter,
                                              unsigned int nPages,
                                              bool fadeIn,
                                              bool fadeOut)
{
    IWUIMenuControllerData menuController;
    menuController.shaderProgram = NULL;
    menuController.presenter = presenter;
    menuController.dataBufferSize = menuController.presenter.triangleBufferData.size;
    menuController.dataBufferStart = (GLfloat*)malloc(menuController.dataBufferSize * sizeof(GLfloat));
    menuController.nPages = nPages;
    menuController.currentPage = 0;
    menuController.nextPage = 0;
    menuController.multiBuffer = IWGRingBufferGen();
    
    menuController.fadeIn = fadeIn;
    menuController.fadeOut = fadeOut;
    menuController.fadeInTime = 0.3;// [0.3]
    menuController.fadeOutTime = 0.3;// [0.3]
    
    menuController.isInteractive = true;
    menuController.status = IWUIMENUCONTROLLER_STATUS_INTERACTIVE;
    
    menuController.textColorTransition
    = IWVector4TransitionMake(menuController.presenter.color,
                              menuController.presenter.color,
                              menuController.presenter.color,
                              menuController.fadeInTime,
                              0.0, true, false);
    if (menuController.fadeIn) {
        menuController.textColorTransition.transitionHasFinished = false;
        menuController.textColorTransition.startVector.w = 0.0;
        menuController.textColorTransition.currentVector.w = 0.0;
        menuController.presenter.color.w = 0.0;
        menuController.isInteractive = false;
        menuController.status = IWUIMENUCONTROLLER_STATUS_FADE_IN;
    }
    
    if (nPages) {
        menuController.pages = (IWUIMenuPageData*)malloc(nPages * sizeof(IWUIMenuPageData));
        for (unsigned int i = 0; i < nPages; i++) {
            IWUIMenuPageDataSet(&menuController.pages[i], "", NULL, 0, false, NULL);
        }
    } else {
        menuController.pages = NULL;
    }
    
    return menuController;
}

void IWUIMenuControllerFillVBO(IWUIMenuControllerData *menuController,
                               const IWGShaderProgramData *shaderProgram,
                               GLuint textureHandlerId)
{
    menuController->shaderProgram = shaderProgram;
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        IWGRingBufferBind(&menuController->multiBuffer, i);
        
        menuController->multiBuffer.nVertices[i] = menuController->dataBufferSize / 9;
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontMapTextureData);
//        //glGenerateMipmap(GL_TEXTURE_2D);
        
        glBufferData(GL_ARRAY_BUFFER,
                     menuController->dataBufferSize * sizeof(GLfloat),
                     menuController->dataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        IWGShaderProgramEnableVertexAtrribArrays(shaderProgram, 9);
        
//        glEnableVertexAttribArray(shaderProgram->vertexSlot);
//        glVertexAttribPointer(shaderProgram->vertexSlot, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
//        glEnableVertexAttribArray(shaderProgram->colorSlot);
//        glVertexAttribPointer(shaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
//        glEnableVertexAttribArray(shaderProgram->textureOffsetSlot);
//        glVertexAttribPointer(shaderProgram->textureOffsetSlot, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    return;
}

void IWUIMenuControllerUpdate(IWUIMenuControllerData *menuController,
                              bool isTouched,
                              IWPoint2D touchPoint,
                              float timeSinceLastUpdate)
{
    if (menuController->status == IWUIMENUCONTROLLER_STATUS_FADE_IN
        && !menuController->textColorTransition.transitionHasFinished) {
        IWVector4TransitionUpdate(&menuController->textColorTransition,
                                  timeSinceLastUpdate);
        IWUIMenuControllerUpdateTextColor(menuController, menuController->textColorTransition.currentVector);
        if (menuController->textColorTransition.transitionHasFinished) {
            menuController->status = IWUIMENUCONTROLLER_STATUS_INTERACTIVE;
            menuController->isInteractive = true;
        }
    }
    if (menuController->status == IWUIMENUCONTROLLER_STATUS_FADE_OUT_TO_NEW_MENU
        && !menuController->textColorTransition.transitionHasFinished) {
        IWVector4TransitionUpdate(&menuController->textColorTransition,
                                  timeSinceLastUpdate);
        IWUIMenuControllerUpdateTextColor(menuController, menuController->textColorTransition.currentVector);
        if (menuController->textColorTransition.transitionHasFinished) {
            menuController->currentPage = menuController->nextPage;
            if (menuController->fadeIn) {
                menuController->textColorTransition
                = IWVector4TransitionMake(menuController->presenter.color,
                                          menuController->presenter.color,
                                          menuController->presenter.color,
                                          menuController->fadeInTime,
                                          0.0, false, false);
                menuController->textColorTransition.startVector.w = 0.0;
                menuController->textColorTransition.currentVector.w = 0.0;
                menuController->textColorTransition.endVector.w = 1.0;
                //menuController->presenter.color.w = 0.0;
                menuController->isInteractive = false;
                menuController->presenter.color = menuController->textColorTransition.currentVector;
            }
            IWUIMenuControllerPresentMenuPage(menuController,
                                              menuController->currentPage);
            if (menuController->fadeIn) {
                menuController->status = IWUIMENUCONTROLLER_STATUS_FADE_IN;
            } else {
                menuController->status = IWUIMENUCONTROLLER_STATUS_INTERACTIVE;
                menuController->isInteractive = true;
            }
        }
    }
    return;
}

void IWUIMenuControllerPresentMenuPage(IWUIMenuControllerData *menuController,
                                       unsigned int menuPage)
{
    if (menuPage > menuController->nPages - 1) {
        printf("ERROR IWUIMenuControllerPresentMenuPage: Menu page %u out of range\n", menuPage);
    }
    
    if (menuController->fadeOut
        && menuController->status != IWUIMENUCONTROLLER_STATUS_FADE_OUT_TO_NEW_MENU) {
        menuController->status = IWUIMENUCONTROLLER_STATUS_FADE_OUT_TO_NEW_MENU;
        menuController->isInteractive = false;
        menuController->textColorTransition
        = IWVector4TransitionMake(menuController->presenter.color,
                                  menuController->presenter.color,
                                  menuController->presenter.color,
                                  menuController->fadeOutTime,
                                  0.0, false, false);
        menuController->textColorTransition.endVector.w = 0.0;
        menuController->nextPage = menuPage;
        return;
    }
    
    menuController->currentPage = menuPage;
    
    IWIUMenuPresenterPresentMenu(&menuController->presenter,
                                 &menuController->pages[menuController->currentPage]);
    IWGRingBufferSubData(&menuController->multiBuffer,
                         0,
                         menuController->dataBufferSize * sizeof(GLfloat),
                         menuController->dataBufferStart,
                         true);
    return;
}


void IWUIMenuControllerUpdateTextColor(IWUIMenuControllerData *menuController,
                                       IWVector4 color)
{
    menuController->presenter.color = color;
    IWUIMenuPresenterInitTextFields(&menuController->presenter,
                                    menuController->dataBufferStart);
    IWIUMenuPresenterPresentMenu(&menuController->presenter,
                                 &menuController->pages[menuController->currentPage]);
    IWGRingBufferSubData(&menuController->multiBuffer,
                         0,
                         menuController->dataBufferSize * sizeof(GLfloat),
                         menuController->dataBufferStart,
                         true);
    return;
}

void IWUIMenuControllerRender(IWUIMenuControllerData *menuController)
{
    IWGRingBufferBindCurrentDrawBuffer(&menuController->multiBuffer);

#ifndef IW_USE_GLVAO
    if (menuController->shaderProgram) {
        IWGShaderProgramEnableVertexAtrribArrays(menuController->shaderProgram, 9);
    } else {
        printf("ERROR IWUIMenuControllerRender: shaderProgram == NULL\n");
        return;
    }
#endif
    glDrawArrays(GL_TRIANGLES, 0, menuController->multiBuffer.nVertices[menuController->multiBuffer.currentDrawBuffer]);
    
#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(0);
#endif
    
    IWGRingBufferSwitchBuffer(&menuController->multiBuffer);
    
    return;
}

void IWUIMenuControllerPurgeData(IWUIMenuControllerData *menuController)
{
    for (unsigned int i = 0; i < menuController->nPages; i++) {
        IWUIMenuPagePurgeData(&menuController->pages[i]);
    }
    IWUIMenuPresenterPurgeData(&menuController->presenter);
    IWGRingBufferDealloc(&menuController->multiBuffer);
    free(menuController->dataBufferStart);
    return;
}
