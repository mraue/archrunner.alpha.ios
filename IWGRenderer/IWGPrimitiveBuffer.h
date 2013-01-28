//
//  IWGPrimitiveBuffer.h
//  theBr1dge
//
//  Created by Martin Raue on 1/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGPrimitiveBuffer_h
#define theBr1dge_IWGPrimitiveBuffer_h

#include <stdlib.h>
#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

enum {
    IWGPRIMITIVEBUFFERDATA_NEEDS_UPDATE_POSITION  = 1 <<  0,
    IWGPRIMITIVEBUFFERDATA_NEEDS_UPDATE_NORMAL  = 1 <<  1,
    IWGPRIMITIVEBUFFERDATA_NEEDS_UPDATE_COLOR = 1 <<  2,
    IWGPRIMITIVEBUFFERDATA_NEEDS_UPDATE_TEXTUREPOS = 1 <<  3,
    IWGPRIMITIVEBUFFERDATA_NEEDS_UPDATE_ALL = (1 << 4) - 1
};

struct _IWGPrimitiveBufferData {
    size_t size;// [ GLfloat ]
    short stride;// [ GLfloat ]
    GLfloat *startCPU;
    GLfloat *bufferStartCPU;
    unsigned int bufferIDGPU;
    short positionOffset;// [ GLfloat ]
    short normalOffset;// [ GLfloat ]
    short colorOffset;// [ GLfloat ]
    short textureposOffset;// [ GLfloat ]
    int needsUpdate;
};
typedef struct _IWGPrimitiveBufferData IWGPrimitiveBufferData;

IWGPrimitiveBufferData IWGPrimitiveBufferDataMake(size_t size, short stride,
                                                  GLfloat *startCPU, GLfloat *bufferStartCPU, unsigned int bufferIDGPU,
                                                  short positionOffset, short normalOffset, short colorOffset,
                                                  short textureposOffset, int needsUpdate);
IWGPrimitiveBufferData IWGPrimitiveBufferDataMakeEmpty();

bool IWGPrimitiveBufferDataUpdateColor(IWGPrimitiveBufferData *primitiveBufferData, IWVector4 newColor);
bool IWGPrimitiveBufferDataUpdatePosition(IWGPrimitiveBufferData *primitiveBufferData, IWVector3 displacement);

#endif
