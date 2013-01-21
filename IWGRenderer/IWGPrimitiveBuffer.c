//
//  IWGPrimitiveBuffer.c
//  theBr1dge
//
//  Created by Martin Raue on 1/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWGPrimitiveBuffer.h"

IWGPrimitiveBufferData IWGPrimitiveBufferDataMake(size_t size, short stride,
                                                  GLfloat *start, GLfloat *bufferStart,
                                                  short positionOffset, short normalOffset, short colorOffset,
                                                  short textureposOffset, int needsUpdate,
                                                  IWGPrimitiveBufferData* previous,
                                                  IWGPrimitiveBufferData* next)
{
    IWGPrimitiveBufferData primitiveBufferData = {
        size, stride,
        start, bufferStart,
        positionOffset, normalOffset, colorOffset, textureposOffset,
        needsUpdate,
        previous, next
    };
    return primitiveBufferData;
}

IWGPrimitiveBufferData IWGPrimitiveBufferDataMakeEmpty()
{
    return IWGPrimitiveBufferDataMake(0, 0, NULL, NULL, 0, 0, 0, 0, 0, NULL, NULL);
}