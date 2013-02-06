//
//  IWGTexture.h
//  theBr1dge
//
//  Created by Martin Raue on 2/6/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGTexture_h
#define theBr1dge_IWGTexture_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

typedef struct {
    GLuint handlerId;
    void *dataPointer;
    IWVector2 dimensions;
    
} IWGTextureData;

#endif
