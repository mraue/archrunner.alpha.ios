//
//  IWGPrimitiveBuffer.h
//  theBr1dge
//
//  Created by Martin Raue on 1/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGPrimitiveBuffer_h
#define theBr1dge_IWGPrimitiveBuffer_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

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
    GLfloat *start;
    GLfloat *bufferStart;
    short positionOffset;
    short normalOffset;
    short colorOffset;
    short textureposOffset;
    int needsUpdate;
    struct _IWGPrimitiveBufferData *previous;
    struct _IWGPrimitiveBufferData *next;
};
typedef struct _IWGPrimitiveBufferData IWGPrimitiveBufferData;

IWGPrimitiveBufferData IWGPrimitiveBufferDataMake(size_t size, short stride,
                                                  GLfloat *start, GLfloat *bufferStart,
                                                  short positionOffset, short normalOffset, short colorOffset,
                                                  short textureposOffset, int needsUpdate,
                                                  IWGPrimitiveBufferData* previous,
                                                  IWGPrimitiveBufferData* next);
IWGPrimitiveBufferData IWGPrimitiveBufferDataMakeEmpty();

#endif
