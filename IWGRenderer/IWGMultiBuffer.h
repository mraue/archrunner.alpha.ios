//
//  IWGMultiBuffer.h
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGMultiBuffer_h
#define theBr1dge_IWGMultiBuffer_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWGBufferSubData.h"

#define IWGMULTIBUFFER_MAX 3

typedef struct {
    unsigned int currentDrawBuffer;
    unsigned int currentDataUpdateBuffer;
    GLuint nVertices[IWGMULTIBUFFER_MAX];
    GLuint vertexArray[IWGMULTIBUFFER_MAX];
    GLuint vertexBuffer[IWGMULTIBUFFER_MAX];
    IWGBufferSubData *bufferSubData[IWGMULTIBUFFER_MAX];
} IWGMultiBufferData;

IWGMultiBufferData IWGMultiBufferGen(void);

void IWGMultiBufferBind(IWGMultiBufferData *multiBufferData, unsigned int buffer);

void IWGMultiBufferBindCurrentDrawBuffer(IWGMultiBufferData *multiBufferData);
void IWGMultiBufferBindCurrentDataUpdateBuffer(IWGMultiBufferData *multiBufferData);

void IWGMultiBufferSwitchBuffer(IWGMultiBufferData *multiBufferData);
void IWGMultiBufferSubData(IWGMultiBufferData *multiBufferData,
                            GLintptr offset, GLsizeiptr size, const GLvoid *data,
                            bool bindBuffer);


#endif
