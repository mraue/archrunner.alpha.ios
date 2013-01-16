//
//  IWGHeadUpDisplay.h
//  theBr1dge
//
//  Created by Martin Raue on 1/15/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGHeadUpDisplay_h
#define theBr1dge_IWGHeadUpDisplay_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

enum IWGHEADUPDISPLAY_BUTTON_ANCHOR_POSITION {
    IWGHEADUPDISPLAY_BUTTON_ANCHOR_POSITION_LOWER_LEFT,
    IWGHEADUPDISPLAY_BUTTON_ANCHOR_POSITION_UPPER_LEFT,
    IWGHEADUPDISPLAY_BUTTON_ANCHOR_POSITION_LOWER_RIGHT,
    IWGHEADUPDISPLAY_BUTTON_ANCHOR_POSITION_UPPER_RIGHT
};

struct _IWGButton {
//    IWVector2 anchorPoint;
//    enum IWGHEADUPDISPLAY_BUTTON_ANCHOR_POSITION anchorPosition;
//    IWVector2 size;
    IWVector2 lowerLeft;
    IWVector2 upperRight;
    IWVector4 colour;    
//    GLfloat *memStartPtr;
//    unsigned short memSize;
};

typedef struct _IWGButton IWGButton;

void IWGHeadUpDisplayButtonToTriangles(IWGButton button, GLfloat *p, int *nVertices);

#endif
