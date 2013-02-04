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
                                                  GLfloat *startCPU, GLfloat *bufferStartCPU,
                                                  unsigned int bufferIDGPU, unsigned int bufferOffsetGPU,
                                                  short positionOffset, short normalOffset, short colorOffset,
                                                  short textureposOffset, int needsUpdate)
{
    IWGPrimitiveBufferData primitiveBufferData = {
        size, stride,
        startCPU, bufferStartCPU, bufferIDGPU, bufferOffsetGPU,
        positionOffset, normalOffset, colorOffset, textureposOffset,
        needsUpdate
    };
    return primitiveBufferData;
}

IWGPrimitiveBufferData IWGPrimitiveBufferDataMakeEmpty()
{
    return IWGPrimitiveBufferDataMake(0, 0, NULL, NULL, 0, 0, 0, 0, 0, 0, 0);
}

bool IWGPrimitiveBufferDataUpdateColor(IWGPrimitiveBufferData *primitiveBufferData, IWVector4 newColor)
{
    if (primitiveBufferData->startCPU) {
        GLfloat *ptr = primitiveBufferData->startCPU;
        ptr += primitiveBufferData->colorOffset;
        while (ptr < primitiveBufferData->startCPU + primitiveBufferData->size) {
            *ptr++ = newColor.x;
            *ptr++ = newColor.y;
            *ptr++ = newColor.z;
            *ptr++ = newColor.w;
            ptr += primitiveBufferData->stride - 4;
        }
    }
    return true;
}

bool IWGPrimitiveBufferDataUpdatePosition(IWGPrimitiveBufferData *primitiveBufferData, IWVector3 displacement)
{
    if (primitiveBufferData->startCPU) {
        GLfloat *ptr = primitiveBufferData->startCPU;
        ptr += primitiveBufferData->positionOffset;
        while (ptr < primitiveBufferData->startCPU + primitiveBufferData->size) {
            *ptr++ += displacement.x;
            *ptr++ += displacement.y;
            *ptr++ += displacement.z;
            ptr += primitiveBufferData->stride - 3;
        }
    }
    return true;
}