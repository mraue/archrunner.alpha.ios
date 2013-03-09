//
//  IWUIMenu.h
//  theBr1dge
//
//  Created by Martin Raue on 2/20/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIMenu_h
#define theBr1dge_IWUIMenu_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

#include "IWVectorTransition.h"

#include "IWGRingBuffer.h"

#include "IWUIMenuPresenter.h"
#include "IWUIMenuPage.h"

typedef enum {
    IWUIMENUCONTROLLER_STATUS_INTERACTIVE,
    IWUIMENUCONTROLLER_STATUS_FADE_IN,
    IWUIMENUCONTROLLER_STATUS_FADE_OUT,
    IWUIMENUCONTROLLER_STATUS_FADE_OUT_TO_NEW_MENU
} IWUIMENUCONTROLLER_STATUS;

typedef struct {
    IWUIMenuPresenterData presenter;
    IWUIMenuPageData *pages;
    unsigned int nPages;
    unsigned int currentPage;
    size_t dataBufferSize;
    GLfloat* dataBufferStart;
    IWGRingBufferData multiBuffer;
    bool fadeIn, fadeOut;
    float fadeInTime, fadeOutTime;
    IWVector4Transition textColorTransition;
    bool isInteractive;
    IWUIMENUCONTROLLER_STATUS status;
    unsigned int nextPage;
} IWUIMenuControllerData;

IWUIMenuControllerData IWUIMenuControllerMake(IWUIMenuPresenterData presenter,
                                              unsigned int nPages,
                                              bool fadeIn,
                                              bool fadeOut);

void IWUIMenuControllerFillVBO(IWUIMenuControllerData *menuController,
                               GLuint positionSlot,
                               GLuint colorSlot,
                               GLuint textureOffsetSlot,
                               GLuint textureHandlerId,
                               GLvoid* fontMapTextureData);

void IWUIMenuControllerUpdate(IWUIMenuControllerData *menuController,
                              bool isTouched,
                              IWPoint2D touchPoint,
                              float timeSinceLastUpdate);

void IWUIMenuControllerPresentMenuPage(IWUIMenuControllerData *menuController,
                                       unsigned int menuPage);

void IWUIMenuControllerUpdateTextColor(IWUIMenuControllerData *menuController,
                                       IWVector4 color);

void IWUIMenuControllerRender(IWUIMenuControllerData *menuController);

void IWUIMenuControllerPurgeData(IWUIMenuControllerData *menuController);

#endif
