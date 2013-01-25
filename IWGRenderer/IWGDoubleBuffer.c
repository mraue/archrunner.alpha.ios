//
//  IWGDoubleBuffer.c
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGDoubleBuffer.h"

#include <stdio.h>
#include <stdlib.h>

IWGDoubleBufferData IWGDoubleBufferGen(void)
{
    IWGDoubleBufferData doubleBufferData;
    for (unsigned int i = 0; i < IWGDOUBLEBUFFER_MAX; i++) {
        glGenVertexArraysOES(1, &doubleBufferData.vertexArray[i]);
        glGenBuffers(1, &doubleBufferData.vertexBuffer[i]);
        doubleBufferData.bufferSubData[i] = NULL;
        doubleBufferData.nVertices[i] = 0;
    }
    doubleBufferData.currentBuffer = 0;
    return doubleBufferData;
}

void IWGDoubleBufferBind(IWGDoubleBufferData *doubleBufferData, unsigned int buffer)
{
    glBindVertexArrayOES(doubleBufferData->vertexArray[buffer]);
    glBindBuffer(GL_ARRAY_BUFFER, doubleBufferData->vertexBuffer[buffer]);
    return;
}

void IWGDoubleBufferBindCurrentBuffer(IWGDoubleBufferData *doubleBufferData)
{
    IWGDoubleBufferBind(doubleBufferData, doubleBufferData->currentBuffer);
    return;
}

void IWGDoubleBufferSwitchBuffer(IWGDoubleBufferData *doubleBufferData)
{
    unsigned int previousBuffer = doubleBufferData->currentBuffer;
    unsigned int currentBuffer = doubleBufferData->currentBuffer += previousBuffer % IWGDOUBLEBUFFER_MAX;
    IWGDoubleBufferBindCurrentBuffer(doubleBufferData);
    IWGBufferSubData *subData = NULL;
    while ((subData = doubleBufferData->bufferSubData[currentBuffer])) {
        glBufferSubData(GL_ARRAY_BUFFER, subData->offset, subData->size, subData->data);
        if (subData->next) {
            doubleBufferData->bufferSubData[currentBuffer] = subData->next;
            free(subData);
        } else {
            free(subData);
            doubleBufferData->bufferSubData[currentBuffer] = NULL;
        }
    }
    doubleBufferData->nVertices[currentBuffer] = doubleBufferData->nVertices[previousBuffer];
}

void IWGDoubleBufferSubData(IWGDoubleBufferData *doubleBufferData,
                            GLintptr offset, GLsizeiptr size, const GLvoid *data,
                            bool bindBuffer)
{
    if (bindBuffer)
        IWGDoubleBufferBindCurrentBuffer(doubleBufferData);
    
    for (unsigned int i = 0; i < IWGDOUBLEBUFFER_MAX; i++) {
        if (i == doubleBufferData->currentBuffer) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        } else {
            IWGBufferSubData *bufferSubData = IWGBufferSubDataMake(offset, size, data);
            if (doubleBufferData->bufferSubData[i]) {
                IWGBufferSubDataAppend(doubleBufferData->bufferSubData[i], bufferSubData);
            } else {
                doubleBufferData->bufferSubData[i] = bufferSubData;
            }
        }
    }
}
