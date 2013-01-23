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

enum _IWUI_ORIENTATION {
    IWUI_ORIENTATION_HORIZONTAL,
    IWUI_ORIENTATION_VERTICAL
};
typedef enum _IWUI_ORIENTATION IWUI_ORIENTATION;

enum _IWUI_DIRECTION {
    IWUI_DIRECTION_TO_LEFT,
    IWUI_DIRECTION_TO_RIGHT,
    IWUI_DIRECTION_TO_UP,
    IWUI_DIRECTION_TO_DOWN
};
typedef enum _IWUI_ORIENTATION IWUI_DIRECTION;

struct _IWUIElementData {
    IWRectangle rectangle;// [0.0 - 1.0]
    size_t nTriangleVertices;
    GLfloat *triangleBufferStart;
    size_t triangleBufferSize;// [GLFloat]
    size_t nLineVertices;
    GLfloat *lineBufferStart;
    size_t lineBufferSize;// [GLFloat]
};
typedef struct _IWUIElementData IWUIElementData;

IWUIElementData IWUIElementMakeEmpty();

IWUIElementData IWUIElementMakeCircle(IWPoint centerPoint, float radiusX, IWVector4 color,
                                      float aspectRatio, unsigned int nDivisions, GLfloat* bufferPointer);


#endif
