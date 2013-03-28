//
//  IWGBufferSubData.h
//  theBr1dge
//
//  Created by Martin Raue on 1/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGBufferSubData_h
#define theBr1dge_IWGBufferSubData_h

#include "IWOpenGL.h"

struct _IWGBufferSubData {
    GLintptr offset;
    GLsizeiptr size;
    const GLvoid *data;
    struct _IWGBufferSubData *next;    
};
typedef struct _IWGBufferSubData IWGBufferSubData;

IWGBufferSubData* IWGBufferSubDataMake(GLintptr offset, GLsizeiptr size, const GLvoid *data);
void IWGBufferSubDataAppend(IWGBufferSubData *bufferSubData, IWGBufferSubData *bufferSubDataAppend);

//glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)

#endif
