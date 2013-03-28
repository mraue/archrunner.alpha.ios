//
//  IWGRingBuffer.c
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGRingBuffer.h"

#include <stdio.h>
#include <stdlib.h>

IWGRingBufferData IWGRingBufferGen(void)
{
    IWGRingBufferData multiBufferData;
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
#ifdef IW_USE_GLVAO
        glGenVertexArraysOES(1, &multiBufferData.vertexArray[i]);
#endif
        glGenBuffers(1, &multiBufferData.vertexBuffer[i]);
        multiBufferData.bufferSubData[i] = NULL;
        multiBufferData.nVertices[i] = 0;
    }
    multiBufferData.currentDataUpdateBuffer = 0;
    multiBufferData.currentDrawBuffer = 1 % IWGMULTIBUFFER_MAX;
    return multiBufferData;
}

void IWGRingBufferBind(IWGRingBufferData *multiBufferData, unsigned int buffer)
{
#ifdef IW_USE_GLVAO
    glBindVertexArrayOES(multiBufferData->vertexArray[buffer]);
#endif
    glBindBuffer(GL_ARRAY_BUFFER, multiBufferData->vertexBuffer[buffer]);
    return;
}

void IWGRingBufferBindCurrentDrawBuffer(IWGRingBufferData *multiBufferData)
{
    IWGRingBufferBind(multiBufferData, multiBufferData->currentDrawBuffer);
    return;
}

void IWGRingBufferBindCurrentDataUpdateBuffer(IWGRingBufferData *multiBufferData)
{
    IWGRingBufferBind(multiBufferData, multiBufferData->currentDataUpdateBuffer);
    return;
}

void IWGRingBufferSwitchBuffer(IWGRingBufferData *multiBufferData)
{
    unsigned int previousBuffer = multiBufferData->currentDataUpdateBuffer;
    unsigned int currentBuffer = multiBufferData->currentDataUpdateBuffer = (previousBuffer + 1) % IWGMULTIBUFFER_MAX;
    
    IWGRingBufferBindCurrentDataUpdateBuffer(multiBufferData);
    
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
    multiBufferData->currentDrawBuffer = (currentBuffer + IWGMULTIBUFFER_MAX / 2) % IWGMULTIBUFFER_MAX;
}

void IWGRingBufferSubData(IWGRingBufferData *multiBufferData,
                           GLintptr offset, GLsizeiptr size, const GLvoid *data,
                           bool bindBuffer)
{
    if (bindBuffer)
        IWGRingBufferBindCurrentDataUpdateBuffer(multiBufferData);
    
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

void IWGRingBufferSubDataForBufferObject(IWGRingBufferData *multiBuffer,
                                          IWGPrimitiveBufferData *primitiveBuffer,
                                          bool bindBuffer)
{
    IWGRingBufferSubData(multiBuffer,
                          primitiveBuffer->bufferOffsetGPU  * sizeof(GLfloat),
                          primitiveBuffer->size * sizeof(GLfloat),
                          primitiveBuffer->bufferStartCPU,
                          bindBuffer);
    return;
}

void IWGRingBufferPurgeBufferSubData(IWGRingBufferData *multiBufferData)
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

void IWGRingBufferResetNVertices(IWGRingBufferData *multiBufferData)
{
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        multiBufferData->nVertices[i] = 0;
    }
}

void IWGRingBufferDealloc(IWGRingBufferData *multiBufferData)
{
    IWGRingBufferPurgeBufferSubData(multiBufferData);
    IWGRingBufferResetNVertices(multiBufferData);
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        glDeleteBuffers(1, &multiBufferData->vertexBuffer[i]);
#ifdef IW_USE_GLVAO
        glDeleteVertexArraysOES(1, &multiBufferData->vertexArray[i]);
#endif
    }
}
