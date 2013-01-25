//
//  IWGDoubleBuffer.h
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGDoubleBuffer_h
#define theBr1dge_IWGDoubleBuffer_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWGBufferSubData.h"

#define IWGDOUBLEBUFFER_MAX 2

struct _IWGDoubleBufferData {
    unsigned int currentBuffer;
    GLuint nVertices[IWGDOUBLEBUFFER_MAX];
    GLuint vertexArray[IWGDOUBLEBUFFER_MAX];
    GLuint vertexBuffer[IWGDOUBLEBUFFER_MAX];
    IWGBufferSubData *bufferSubData[IWGDOUBLEBUFFER_MAX];
};
typedef struct _IWGDoubleBufferData IWGDoubleBufferData;

IWGDoubleBufferData IWGDoubleBufferGen(void);

void IWGDoubleBufferBind(IWGDoubleBufferData *doubleBufferData, unsigned int buffer);
void IWGDoubleBufferBindCurrentBuffer(IWGDoubleBufferData *doubleBufferData);

void IWGDoubleBufferSwitchBuffer(IWGDoubleBufferData *doubleBufferData);
void IWGDoubleBufferSubData(IWGDoubleBufferData *doubleBufferData,
                            GLintptr offset, GLsizeiptr size, const GLvoid *data,
                            bool bindBuffer);


#endif
