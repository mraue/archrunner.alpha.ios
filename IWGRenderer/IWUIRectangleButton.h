//
//  IWUIRectangleButton.h
//  theBr1dge
//
//  Created by Martin Raue on 1/16/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIRectangleButton_h
#define theBr1dge_IWUIRectangleButton_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWGeometry.h"
#include "IWColorTransition.h"

enum IWUIRECTANGLEBUTTON_CORNER_CUT {
    IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_LEFT  = 1 <<  0,
    IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT  = 1 <<  1,
    IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_RIGHT = 1 <<  2,
    IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT = 1 <<  3,
    IWUIRECTANGLEBUTTON_CORNER_CUT_ALL = (1 << 4) - 1
};

struct _IWUIRectangleButton {
    IWPoint anchorPoint;// [0.0, 1.0] coordinates
    enum IWRECTANGLE_ANCHOR_POSITION anchorPosition;
    IWVector2 size;// [0.0, 1.0] coordinates
    IWVector4 color;
    enum IWUIRECTANGLEBUTTON_CORNER_CUT cornerCut;
    float cornerOffset;
    float aspectRatio;
    IWRectangle rectangle;// [0.0 - 1.0]
    unsigned int nVertices;
    GLfloat *memStartPtr;
    size_t memSize;// [CGFloat]
    IWColorTransition colorTransition;
    bool isTouched;
};

typedef struct _IWUIRectangleButton IWUIRectangleButton;

IWUIRectangleButton IWUIRectangleButtonMake(float anchorPointX, float anchorPointY,
                                          enum IWRECTANGLE_ANCHOR_POSITION anchorPosition,
                                          float sizeX, float sizeY,
                                          IWVector4 color,
                                          enum IWUIRECTANGLEBUTTON_CORNER_CUT cornerCut,
                                          float cornerOffset, float aspectRatio);

size_t IWUIRectangleButtonMemorySize(IWUIRectangleButton *button);//[CGFloat]

size_t IWUIRectangleButtonToTriangleBuffer(IWUIRectangleButton *button, GLfloat* p);

void IWUIRectangleButtonUpdateColorInBuffer(IWUIRectangleButton *button);

#endif
