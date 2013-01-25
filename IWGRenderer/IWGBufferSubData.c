//
//  IWGBufferSubData.c
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGBufferSubData.h"

#include <stdio.h>
#include <stdlib.h>

IWGBufferSubData* IWGBufferSubDataMake(GLintptr offset, GLsizeiptr size, const GLvoid *data)
{
    IWGBufferSubData *bufferSubData = (IWGBufferSubData*)malloc(sizeof(IWGBufferSubData));
    bufferSubData->offset = offset;
    bufferSubData->size = size;
    bufferSubData->data = data;
    bufferSubData->next = NULL;
    return bufferSubData;
}

void IWGBufferSubDataAppend(IWGBufferSubData *bufferSubData, IWGBufferSubData *bufferSubDataAppend)
{
    while (bufferSubData->next) {
        bufferSubData = bufferSubData->next;
    }
    bufferSubData->next = bufferSubDataAppend;
    return;
}