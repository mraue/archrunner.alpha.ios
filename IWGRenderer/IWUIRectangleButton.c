//
//  IWUIRectangleButton.c
//  theBr1dge
//
//  Created by Martin Raue on 1/16/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWUIRectangleButton.h"

IWUIRectangleButton IWUIRectangleButtonMake(float anchorPointX, float anchorPointY,
                                            enum IWRECTANGLE_ANCHOR_POSITION anchorPosition,
                                            float sizeX, float sizeY,
                                            IWVector4 touchedColor, IWVector4 untouchedColor,
                                            enum IWUIRECTANGLEBUTTON_CORNER_CUT cornerCut,
                                            float cornerOffset, float aspectRatio)
{
    IWUIRectangleButton button = {
        {anchorPointX, anchorPointY},
        anchorPosition,
        {sizeX, sizeY},
        touchedColor,
        untouchedColor,
        untouchedColor,
        cornerCut,
        cornerOffset,
        aspectRatio,
        {{0.0, 0.0}, {0.0, 0.0}},
        0, 0, 0,
        IWColorTransitionMake(), false
    };
    button.rectangle = IWRectangleMakeFromAnchorAndDimensions(button.anchorPoint,
                                                              IWVector2Make(sizeX, sizeY),
                                                              button.anchorPosition);
    return button;
}

size_t IWUIRectangleButtonMemorySize(IWUIRectangleButton *button)
{
    size_t memSize = 4 * 6;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_LEFT)
        memSize += 3;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_RIGHT)
        memSize += 3;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT)
        memSize += 3;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT)
        memSize += 3;
    return memSize * 7;
}

bool IWUIRectangleButtonCheckTouch(IWUIRectangleButton *button, bool isTouched, IWPoint touchPoint)
{
    if (isTouched && IWPointInRectangle(touchPoint, button->rectangle)) {
        if (!button->isTouched) {
            button->isTouched = true;
            IWColorTransition colorTransition = {
                button->color,
                button->touchedColor,
                button->color,
                0.1, 0.0, false, true
            };
            button->colorTransition = colorTransition;
        }
        return true;
    } else {
        if (button->isTouched) {
            button->isTouched = false;
            IWColorTransition colorTransition = {
                button->color,
                button->untouchedColor,
                button->color,
                0.2, 0.0, false, false
            };
            button->colorTransition = colorTransition;
        }
        return false;
    }
}

bool IWUIRectangleButtonPointInRectangle(IWUIRectangleButton *button, IWVector2 point)
{
    return ((point.x >= button->rectangle.lowerLeft.x) && (point.x < button->rectangle.upperRight.x)
            && (point.y >= button->rectangle.lowerLeft.y) && (point.y < button->rectangle.upperRight.y));
}

static __inline__ size_t IWUIRectangleButtonBufferAppendVertex(GLfloat *p, IWVector2 xyvec, float z, IWVector4 color)
{
    GLfloat *pstart = p;
    *p++ = xyvec.x; *p++ = xyvec.y; *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    return p - pstart;
}

// Duplicated here to have full controll over the order,
// since it must match the index array
const static short _IWUIRECTANGLEBUTTONTOTRIANGLEBUFFER_CORNER_CUT[] = {
    IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_LEFT,
    IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_RIGHT,
    IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT,
    IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT
};

// Helper structure
struct _IWUIRECTANGLEBUTTONTOTRIANGLEBUFFER_INDICES_STRUCT {
    unsigned short cornerCut[9];
    unsigned short noCornerCut[6];
};

// Indices for the different quadrants of the button, with
// and without the corner cut
static struct _IWUIRECTANGLEBUTTONTOTRIANGLEBUFFER_INDICES_STRUCT _IWUIRECTANGLEBUTTONTOTRIANGLEBUFFER_INDICES[] = {
    {{2, 3, 0, 2, 0, 16, 16, 0, 15}, {1, 3, 0, 1, 0, 15}},
    {{3, 4, 0, 4, 6, 0, 6, 7, 0}, {3, 5, 0, 5, 7, 0}},
    {{0, 7, 8, 0, 8, 10, 0, 10, 11}, {0, 7, 9, 0, 9, 11}},
    {{0, 11, 12, 0, 12, 14, 0, 14, 15}, {0, 11, 13, 0, 13, 15}}
};

size_t IWUIRectangleButtonToTriangleBuffer(IWUIRectangleButton * button, GLfloat* p)
{
    //float* p = &vertices->x;
    button->memStartPtr = p;
    
    IWVector4 color = button->color;
    //float cornerOffset = (button->rectangle.upperRight.x - button->rectangle.lowerLeft.x) * button->cornerCutXFraction;

    // REFACTOR: Aspect ratio needs to be checked, and implementation needs to be improved
    IWVector2 cornerOffset = IWVector2MultiplyScalar(IWVector2Make(1., button->aspectRatio), button->cornerOffset * 2.);
    IWVector2 center = IWVector2MultiplyScalar(IWVector2Add(button->rectangle.upperRight, button->rectangle.lowerLeft), 0.5);

    // Convert to GL coordinates
    // REFACTOR: this shall be improved in the future
    float xc, yc, xmin, xmax, ymin, ymax;
    xc = center.x * 2.0 - 1.0;
    yc = center.y * 2.0 - 1.0;
    xmin = button->rectangle.lowerLeft.x * 2.0 - 1.0;
    xmax = button->rectangle.upperRight.x * 2.0 - 1.0;
    ymin = button->rectangle.lowerLeft.y * 2.0 - 1.0;
    ymax = button->rectangle.upperRight.y * 2.0 - 1.0;

    // Calculate base vertices
    IWVector2 baseVertices[] = {
        {xc, yc},
        {xmin, ymin}, {xmin + cornerOffset.x, ymin}, {xc, ymin}, {xmax - cornerOffset.x, ymin}, {xmax, ymin},
        {xmax, ymin + cornerOffset.y}, {xmax, yc}, {xmax, ymax - cornerOffset.y}, {xmax, ymax},
        {xmax - cornerOffset.x, ymax}, {xc, ymax}, {xmin + cornerOffset.x, ymax}, {xmin, ymax},
        {xmin, ymax - cornerOffset.y}, {xmin, yc}, {xmin, ymin + cornerOffset.y}
    };
    
    unsigned short *indices;
    for (int k = 0; k < 4; k++) {
        int imax = 9;
        if (button->cornerCut & _IWUIRECTANGLEBUTTONTOTRIANGLEBUFFER_CORNER_CUT[k]) {
            indices = _IWUIRECTANGLEBUTTONTOTRIANGLEBUFFER_INDICES[k].cornerCut;
        } else {
            indices = _IWUIRECTANGLEBUTTONTOTRIANGLEBUFFER_INDICES[k].noCornerCut;
            imax = 6;
        }
        for (int i = 0; i < imax; i++) {
            p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[indices[i]], -1.0, color);
        }
    }
    button->memSize = p - button->memStartPtr;
    button->nVertices = button->memSize / 7;
    return button->memSize;
}

void IWUIRectangleButtonUpdateColorInBuffer(IWUIRectangleButton *button)
{
    if (button->memStartPtr) {
        GLfloat *ptr = button->memStartPtr;
        while (ptr < button->memStartPtr + button->memSize) {
            ptr += 3;
            *ptr++ = button->color.x;
            *ptr++ = button->color.y;
            *ptr++ = button->color.z;
            *ptr++ = button->color.w;
        }
    }
}