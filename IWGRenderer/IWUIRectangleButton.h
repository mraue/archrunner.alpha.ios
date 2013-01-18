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
    IWVector4 touchedColor;
    IWVector4 untouchedColor;
    IWVector4 lineColor;
    IWVector4 color;
    enum IWUIRECTANGLEBUTTON_CORNER_CUT cornerCut;
    float cornerOffset;
    float aspectRatio;
    IWRectangle rectangle;// [0.0 - 1.0]
    size_t nTriangleVertices;
    GLfloat *triangleBufferStart;
    size_t triangleBufferSize;// [CGFloat]
    size_t nLineVertices;
    GLfloat *lineBufferStart;
    size_t lineBufferSize;// [CGFloat]
    IWColorTransition colorTransition;
    bool isTouched;
};

typedef struct _IWUIRectangleButton IWUIRectangleButton;

IWUIRectangleButton IWUIRectangleButtonMake(float anchorPointX, float anchorPointY,
                                            enum IWRECTANGLE_ANCHOR_POSITION anchorPosition,
                                            float sizeX, float sizeY,
                                            IWVector4 touchedColor, IWVector4 untouchedColor,
                                            IWVector4 lineColor,
                                            enum IWUIRECTANGLEBUTTON_CORNER_CUT cornerCut,
                                            float cornerOffset, float aspectRatio);

size_t IWUIRectangleButtonTriangleBufferSize(IWUIRectangleButton *button);//[CGFloat]
size_t IWUIRectangleButtonLineBufferSize(IWUIRectangleButton *button);//[CGFloat]

bool IWUIRectangleButtonCheckTouch(IWUIRectangleButton * button, bool isTouched, IWPoint touchPoint);

size_t IWUIRectangleButtonToTriangleBuffer(IWUIRectangleButton *button, GLfloat* p);
size_t IWUIRectangleButtonToLineBuffer(IWUIRectangleButton *button, GLfloat* p);

void IWUIRectangleButtonUpdateColorInBuffer(IWUIRectangleButton *button);

#endif
