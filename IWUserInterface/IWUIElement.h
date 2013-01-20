//
//  IWUIElement.h
//  theBr1dge
//
//  Created by Martin Raue on 1/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIElement_h
#define theBr1dge_IWUIElement_h

#include <stdio.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWGeometry.h"

struct _IWUIElement {
    IWRectangle rectangle;// [0.0 - 1.0]
    size_t nTriangleVertices;
    GLfloat *triangleBufferStart;
    size_t triangleBufferSize;// [CGFloat]
    size_t nLineVertices;
    GLfloat *lineBufferStart;
    size_t lineBufferSize;// [CGFloat]
};
typedef struct _IWUIElement IWUIElement;

IWUIElement IWUIElementMakeCircle(IWPoint centerPoint, float radiusX, IWVector4 color,
                                  float aspectRatio, unsigned int nDivisions, GLfloat* bufferPointer);


#endif
