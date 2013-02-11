//
//  IWGMultiBuffer.c
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGMultiBuffer.h"

#include <stdio.h>
#include <stdlib.h>

IWGMultiBufferData IWGMultiBufferGen(void)
{
    IWGMultiBufferData multiBufferData;
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        glGenVertexArraysOES(1, &multiBufferData.vertexArray[i]);
        glGenBuffers(1, &multiBufferData.vertexBuffer[i]);
        multiBufferData.bufferSubData[i] = NULL;
        multiBufferData.nVertices[i] = 0;
    }
    multiBufferData.currentDataUpdateBuffer = 0;
    multiBufferData.currentDrawBuffer = 1 % IWGMULTIBUFFER_MAX;
    return multiBufferData;
}

void IWGMultiBufferBind(IWGMultiBufferData *multiBufferData, unsigned int buffer)
{
    glBindVertexArrayOES(multiBufferData->vertexArray[buffer]);
    glBindBuffer(GL_ARRAY_BUFFER, multiBufferData->vertexBuffer[buffer]);
    return;
}

void IWGMultiBufferBindCurrentDrawBuffer(IWGMultiBufferData *multiBufferData)
{
    IWGMultiBufferBind(multiBufferData, multiBufferData->currentDrawBuffer);
    return;
}

void IWGMultiBufferBindCurrentDataUpdateBuffer(IWGMultiBufferData *multiBufferData)
{
    IWGMultiBufferBind(multiBufferData, multiBufferData->currentDataUpdateBuffer);
    return;
}

void IWGMultiBufferSwitchBuffer(IWGMultiBufferData *multiBufferData)
{
    unsigned int previousBuffer = multiBufferData->currentDataUpdateBuffer;
    unsigned int currentBuffer = multiBufferData->currentDataUpdateBuffer = (previousBuffer + 1) % IWGMULTIBUFFER_MAX;
    
    IWGMultiBufferBindCurrentDataUpdateBuffer(multiBufferData);
    
    IWGBufferSubData *subData = NULL;
    while ((subData = multiBufferData->bufferSubData[currentBuffer])) {
        glBufferSubData(GL_ARRAY_BUFFER, subData->offset, subData->size, subData->data);
        if (subData->next) {
            multiBufferData->bufferSubData[currentBuffer] = subData->next;
            free(subData);
        } else {
            free(subData);
            multiBufferData->bufferSubData[currentBuffer] = NULL;
        }
    }
    multiBufferData->nVertices[currentBuffer] = multiBufferData->nVertices[previousBuffer];
    multiBufferData->currentDrawBuffer = (currentBuffer + 1) % IWGMULTIBUFFER_MAX;
}

void IWGMultiBufferSubData(IWGMultiBufferData *multiBufferData,
                           GLintptr offset, GLsizeiptr size, const GLvoid *data,
                           bool bindBuffer)
{
    if (bindBuffer)
        IWGMultiBufferBindCurrentDataUpdateBuffer(multiBufferData);
    
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        if (i == multiBufferData->currentDataUpdateBuffer) {
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
        } else {
            IWGBufferSubData *bufferSubData = IWGBufferSubDataMake(offset, size, data);
            if (multiBufferData->bufferSubData[i]) {
                IWGBufferSubDataAppend(multiBufferData->bufferSubData[i], bufferSubData);
            } else {
                multiBufferData->bufferSubData[i] = bufferSubData;
            }
        }
    }
}

void IWGMultiBufferSubDataForBufferObject(IWGMultiBufferData *multiBuffer,
                                          IWGPrimitiveBufferData *primitiveBuffer,
                                          bool bindBuffer)
{
    IWGMultiBufferSubData(multiBuffer,
                          primitiveBuffer->bufferOffsetGPU  * sizeof(GLfloat),
                          primitiveBuffer->size * sizeof(GLfloat),
                          primitiveBuffer->bufferStartCPU,
                          bindBuffer);
    return;
}

void IWGMultiBufferPurgeBufferSubData(IWGMultiBufferData *multiBufferData)
{
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        IWGBufferSubData *bufferSubDataNext = NULL;
        IWGBufferSubData *bufferSubData = multiBufferData->bufferSubData[i];
        while (bufferSubData) {
            bufferSubDataNext = bufferSubData->next;
            free(bufferSubData);
            bufferSubData = bufferSubDataNext;
        }
        multiBufferData->bufferSubData[i] = NULL;
    }
}
