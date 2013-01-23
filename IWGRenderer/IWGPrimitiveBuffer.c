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
                                                  GLfloat *startCPU, GLfloat *bufferStartCPU, unsigned int bufferIDGPU,
                                                  short positionOffset, short normalOffset, short colorOffset,
                                                  short textureposOffset, int needsUpdate)
{
    IWGPrimitiveBufferData primitiveBufferData = {
        size, stride,
        startCPU, bufferStartCPU, bufferIDGPU,
        positionOffset, normalOffset, colorOffset, textureposOffset,
        needsUpdate
    };
    return primitiveBufferData;
}

IWGPrimitiveBufferData IWGPrimitiveBufferDataMakeEmpty()
{
    return IWGPrimitiveBufferDataMake(0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0);
}