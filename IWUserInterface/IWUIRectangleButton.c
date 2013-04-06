//
//  IWUIRectangleButton.c
//  theBr1dge
//
//  Created by Martin Raue on 1/16/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWUIRectangleButton.h"

#define IWUIRECTANGLEBUTTON_TRIANGLE_Z -0.999
#define IWUIRECTANGLEBUTTON_LINE_Z -0.9999

IWUIRectangleButton IWUIRectangleButtonMake(float anchorPointX, float anchorPointY,
                                            IWGEOMETRY_ANCHOR_POSITION anchorPosition,
                                            float sizeX, float sizeY,
                                            IWVector4 touchedColor, IWVector4 untouchedColor,
                                            IWVector4 lineColor,
                                            enum IWUIRECTANGLEBUTTON_CORNER_CUT cornerCut,
                                            float cornerOffset, float aspectRatio)
{
    IWUIRectangleButton button = {
        {anchorPointX, anchorPointY},
        anchorPosition,
        {sizeX, sizeY},
        touchedColor,
        untouchedColor,
        lineColor,
        untouchedColor,
        cornerCut,
        cornerOffset,
        aspectRatio,
        IWRectangleMakeFromAnchorAndDimensions(IWVector2Make(anchorPointX, anchorPointY),
                                               IWVector2Make(sizeX, sizeY),
                                               anchorPosition),
        IWGPrimitiveBufferDataMake(0, 7, 0, 0, 0, 0, 0, 0, 3, 0, 0),
        IWGPrimitiveBufferDataMake(0, 7, 0, 0, 0, 0, 0, 0, 3, 0, 0),
        IWVector4TransitionMakeEmpty(),
        false
    };
    return button;
}

size_t IWUIRectangleButtonTriangleBufferSize(IWUIRectangleButton *button)
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

size_t IWUIRectangleButtonLineBufferSize(IWUIRectangleButton *button)//[CGFloat]
{
    size_t memSize = 16;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_LEFT)
        memSize += 2;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_RIGHT)
        memSize += 2;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT)
        memSize += 2;
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT)
        memSize += 2;
    return memSize * 7;
}

bool IWUIRectangleButtonCheckTouch(IWUIRectangleButton *button, bool isTouched, IWPoint2D touchPoint)
{
    if (isTouched && IWPointInRectangle(touchPoint, button->rectangle)) {
        if (!button->isTouched) {
            button->isTouched = true;
            IWVector4Transition colorTransition = {
                button->color,
                button->touchedColor,
                button->color,
                0.1, 0.0, false, true
            };
            button->colorTransition = colorTransition;
            return true;// only once per touch for now
        }
    } else {
        if (button->isTouched) {
            button->isTouched = false;
            IWVector4Transition colorTransition = {
                button->color,
                button->untouchedColor,
                button->color,
                0.2, 0.0, false, false
            };
            button->colorTransition = colorTransition;
        }
    }
    return false;
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
    button->triangleBuffer.bufferStartCPU = p;
    
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
            p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[indices[i]],
                                                       IWUIRECTANGLEBUTTON_TRIANGLE_Z, color);
        }
    }
    button->triangleBuffer.size = p - button->triangleBuffer.bufferStartCPU;
    //button->nTriangleVertices = button->triangleBufferSize / 7;
    return button->triangleBuffer.size;
}

size_t IWUIRectangleButtonToLineBuffer(IWUIRectangleButton *button, GLfloat* p)
{
    button->lineBuffer.startCPU = p;
    
    IWVector4 color = button->lineColor;
    
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

    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[15], IWUIRECTANGLEBUTTON_LINE_Z, color);
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_LEFT) {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[16], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[16], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[2], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[2], IWUIRECTANGLEBUTTON_LINE_Z, color);
    } else {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[1], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[1], IWUIRECTANGLEBUTTON_LINE_Z, color);
    }
    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[3], IWUIRECTANGLEBUTTON_LINE_Z, color);
    
    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[3], IWUIRECTANGLEBUTTON_LINE_Z, color);
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_LOWER_RIGHT) {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[4], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[4], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[6], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[6], IWUIRECTANGLEBUTTON_LINE_Z, color);
    } else {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[5], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[5], IWUIRECTANGLEBUTTON_LINE_Z, color);
    }
    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[7], IWUIRECTANGLEBUTTON_LINE_Z, color);
    
    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[7], IWUIRECTANGLEBUTTON_LINE_Z, color);
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT) {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[8], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[8], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[10], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[10], IWUIRECTANGLEBUTTON_LINE_Z, color);
        
    } else {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[9], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[9], IWUIRECTANGLEBUTTON_LINE_Z, color);
    }
    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[11], IWUIRECTANGLEBUTTON_LINE_Z, color);
    
    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[11], IWUIRECTANGLEBUTTON_LINE_Z, color);
    if (button->cornerCut & IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT) {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[12], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[12], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[14], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[14], IWUIRECTANGLEBUTTON_LINE_Z, color);
        
    } else {
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[13], IWUIRECTANGLEBUTTON_LINE_Z, color);
        p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[13], IWUIRECTANGLEBUTTON_LINE_Z, color);
    }
    p += IWUIRectangleButtonBufferAppendVertex(p, baseVertices[15], IWUIRECTANGLEBUTTON_LINE_Z, color);

    button->lineBuffer.size = p - button->lineBuffer.startCPU;
    //button->nLineVertices = button->lineBufferSize / 7;
    return button->lineBuffer.size;
}

void IWUIRectangleButtonUpdateColorInBuffer(IWUIRectangleButton *button)
{
    if (button->triangleBuffer.bufferStartCPU) {
        GLfloat *ptr = button->triangleBuffer.bufferStartCPU;
        while (ptr < button->triangleBuffer.bufferStartCPU + button->triangleBuffer.size) {
            ptr += 3;
            *ptr++ = button->color.x;
            *ptr++ = button->color.y;
            *ptr++ = button->color.z;
            *ptr++ = button->color.w;
        }
    }
}