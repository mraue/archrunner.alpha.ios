//
//  IWUIMenu.c
//  theBr1dge
//
//  Created by Martin Raue on 2/20/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIMenu.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWUIMenuData IWUIMenuMake(IWUIMenuPresenterData presenter, unsigned int nPages)
{
    IWUIMenuData menu;
    menu.presenter = presenter;
    menu.dataBufferSize = menu.presenter.triangleBufferData.size;
    menu.dataBufferStart = malloc(menu.dataBufferSize * sizeof(GLfloat));
    menu.nPages = nPages;
    
    menu.multiBuffer = IWGRingBufferGen();
    
    if (nPages) {
        menu.pages = malloc(nPages * sizeof(IWUIMenuPageData));
        for (unsigned int i = 0; i < nPages; i++) {
            IWUIMenuPageDataSet(&menu.pages[i], "", NULL, 0, false, NULL);
        }
    } else {
        menu.pages = NULL;
    }

    return menu;
}

void IWUIMenuFillVBO(IWUIMenuData *menu, GLuint positionSlot,GLuint colorSlot, GLuint textureOffsetSlot,
                     GLuint textureHandlerId, GLvoid* fontMapTextureData)
{

    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        IWGRingBufferBind(&menu->multiBuffer, i);
        
        menu->multiBuffer.nVertices[i] = menu->dataBufferSize / 9;
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontMapTextureData);
        //glGenerateMipmap(GL_TEXTURE_2D);
        
        glBufferData(GL_ARRAY_BUFFER,
                     menu->dataBufferSize * sizeof(GLfloat),
                     menu->dataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textureOffsetSlot);
        glVertexAttribPointer(textureOffsetSlot, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    return;
}

void IWUIMenuUpdate(IWUIMenuData *menu, float timeSinceLastUpdate)
{
    return;
}

void IWUIMenuRender(IWUIMenuData *menu)
{
    IWGRingBufferBindCurrentDrawBuffer(&menu->multiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, menu->multiBuffer.nVertices[menu->multiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGRingBufferSwitchBuffer(&menu->multiBuffer);
    
    return;
}

void IWUIMenuPurgeData(IWUIMenuData *menu)
{
    for (unsigned int i = 0; i < menu->nPages; i++) {
        IWUIMenuPagePurgeData(&menu->pages[i]);
    }
    IWUIMenuPresenterPurgeData(&menu->presenter);
    IWGRingBufferDealloc(&menu->multiBuffer);
    free(menu->dataBufferStart);
    return;
}
