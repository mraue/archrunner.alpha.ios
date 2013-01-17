//
//  IWGRectangleButton.h
//  theBr1dge
//
//  Created by Martin Raue on 1/16/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGRectangleButton_h
#define theBr1dge_IWGRectangleButton_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

enum IWGRECTANGLEBUTTON_ANCHOR_POSITION {
    IWGRECTANGLEBUTTON_ANCHOR_POSITION_LOWER_LEFT,
    IWGRECTANGLEBUTTON_ANCHOR_POSITION_UPPER_LEFT,
    IWGRECTANGLEBUTTON_ANCHOR_POSITION_LOWER_RIGHT,
    IWGRECTANGLEBUTTON_ANCHOR_POSITION_UPPER_RIGHT
};

enum IWGRECTANGLEBUTTON_CORNER_CUT {
    IWGRECTANGLEBUTTON_CORNER_CUT_LOWER_LEFT  = 1 <<  0,
    IWGRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT  = 1 <<  1,
    IWGRECTANGLEBUTTON_CORNER_CUT_LOWER_RIGHT = 1 <<  2,
    IWGRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT = 1 <<  3,
    IWGRECTANGLEBUTTON_CORNER_CUT_ALL = (1 << 4) - 1
};

struct _IWGRectangleButton {
    IWVector2 anchorPoint;// [0.0, 1.0] coordinates
    enum IWGRECTANGLEBUTTON_ANCHOR_POSITION anchorPosition;
    IWVector2 size;// [0.0, 1.0] coordinates
    IWVector4 colour;
    enum IWGRECTANGLEBUTTON_CORNER_CUT cornerCut;
    float cornerOffset;
    float aspectRatio;
    IWVector2 lowerLeft;// GL [-1.0, 1.0] coordinates
    IWVector2 upperRight;// GL [-1.0, 1.0] coordinates
    unsigned int nVertices;
    GLfloat *memStartPtr;
    unsigned short memSize;
};

typedef struct _IWGRectangleButton IWGRectangleButton;

IWGRectangleButton IWGRectangleButtonMake(float anchorPointX, float anchorPointY,
                                          enum IWGRECTANGLEBUTTON_ANCHOR_POSITION anchorPosition,
                                          float sizeX, float sizeY,
                                          IWVector4 colour,
                                          enum IWGRECTANGLEBUTTON_CORNER_CUT cornerCut,
                                          float cornerOffset, float aspectRatio);

int IWGRectangleButtonToTriangleBuffer(IWGRectangleButton * button, GLfloat* p);

#endif
