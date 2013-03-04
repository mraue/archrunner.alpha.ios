//
//  IWGRingBuffer.h
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGRingBuffer_h
#define theBr1dge_IWGRingBuffer_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWGBufferSubData.h"

#include "IWGPrimitiveBuffer.h"

#define IWGMULTIBUFFER_MAX 4

typedef struct {
    unsigned int currentDrawBuffer;
    unsigned int currentDataUpdateBuffer;
    GLuint nVertices[IWGMULTIBUFFER_MAX];
    GLuint vertexArray[IWGMULTIBUFFER_MAX];
    GLuint vertexBuffer[IWGMULTIBUFFER_MAX];
    IWGBufferSubData *bufferSubData[IWGMULTIBUFFER_MAX];
} IWGRingBufferData;

IWGRingBufferData IWGRingBufferGen(void);

void IWGRingBufferBind(IWGRingBufferData *multiBufferData, unsigned int buffer);

void IWGRingBufferBindCurrentDrawBuffer(IWGRingBufferData *multiBufferData);
void IWGRingBufferBindCurrentDataUpdateBuffer(IWGRingBufferData *multiBufferData);

// Also bind new data update buffer
void IWGRingBufferSwitchBuffer(IWGRingBufferData *multiBufferData);
void IWGRingBufferSubData(IWGRingBufferData *multiBufferData,
                           GLintptr offset, GLsizeiptr size, const GLvoid *data,
                           bool bindBuffer);

void IWGRingBufferSubDataForBufferObject(IWGRingBufferData *multiBuffer,
                                          IWGPrimitiveBufferData *primitiveBuffer,
                                          bool bindBuffer);

void IWGRingBufferPurgeBufferSubData(IWGRingBufferData *multiBufferData);
void IWGRingBufferResetNVertices(IWGRingBufferData *multiBufferData);
void IWGRingBufferDealloc(IWGRingBufferData *multiBufferData);

#endif
