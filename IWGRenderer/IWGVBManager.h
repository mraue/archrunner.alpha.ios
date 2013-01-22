//
//  IWGVBManager.h
//  theBr1dge
//
//  Created by Martin Raue on 1/22/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGVBManager_h
#define theBr1dge_IWGVBManager_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

struct _IWGVBManagerData {
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint nVertices;
    unsigned int *bufferToObjectMap;
    unsigned int *objectToBufferMap;
    unsigned int mapNEntries;
    unsigned int mapNEntriesMax;
};
typedef struct _IWGVBManagerData IWGVBManagerData;

void IWGVBManagerDataAppendObjectToBuffer(IWGVBManagerData* vbManagerData, unsigned int objectID);
void IWGVBManagerDataReplaceObjectInBufferWithLast(IWGVBManagerData* vbManagerData, unsigned int objectID);

#endif
