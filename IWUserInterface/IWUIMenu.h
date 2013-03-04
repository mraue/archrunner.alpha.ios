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

typedef struct {
    IWUIMenuPresenterData presenter;
    IWUIMenuPageData *pages;
    unsigned int nPages;
    size_t dataBufferSize;
    GLfloat* dataBufferStart;
    IWGRingBufferData multiBuffer;
} IWUIMenuData;

IWUIMenuData IWUIMenuMake(IWUIMenuPresenterData presenter, unsigned int nPages);

void IWUIMenuFillVBO(IWUIMenuData *menu, GLuint positionSlot,GLuint colorSlot, GLuint textureOffsetSlot,
                     GLuint textureHandlerId, GLvoid* fontMapTextureData);

void IWUIMenuUpdate(IWUIMenuData *menu, float timeSinceLastUpdate);

void IWUIMenuRender(IWUIMenuData *menu);

void IWUIMenuPurgeData(IWUIMenuData *menu);

#endif
