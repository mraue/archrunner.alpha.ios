//
//  IWUIElement.c
//  theBr1dge
//
//  Created by Martin Raue on 1/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIElement.h"

#include <math.h>

IWUIElementData IWUIElementMakeEmpty()
{
    IWUIElementData uiElement = {
        {{0.0, 0.0}, {1.0, 1.0}},
        0, NULL, 0, 0, NULL, 0
    };
    return uiElement;
}

IWUIElementData IWUIElementMakeCircle(IWPoint centerPoint, float radiusX, IWVector4 color,
                                      float aspectRatio, unsigned int nDivisions, GLfloat *bufferPointer)
{
    IWUIElementData circle = {
        {{centerPoint.x - radiusX, centerPoint.y - radiusX * aspectRatio},
        {centerPoint.x + radiusX, centerPoint.y + radiusX * aspectRatio}},
        0, NULL, 0, 0, bufferPointer, 0
    };
    float angleStep = 2.0 * M_PI / (nDivisions - 1);
    bool first = true;
    // REFACTOR: This is scaffolded and needs massive refactoring
    for (float angle = 0.0; angle < 2.0 * M_PI; angle += angleStep) {
        *bufferPointer++ = (centerPoint.x + sinf(angle) * radiusX) * 2.0 - 1.0;
        *bufferPointer++ = (centerPoint.y + cosf(angle) * radiusX * aspectRatio * 0.85) * 2.0 - 1.0;
        *bufferPointer++ = -1.0;
        *bufferPointer++ = color.x;
        *bufferPointer++ = color.y;
        *bufferPointer++ = color.z;
        *bufferPointer++ = color.w;
        if (first) {
            first = false;
        } else {
            *bufferPointer++ = (centerPoint.x + sinf(angle) * radiusX) * 2.0 - 1.0;
            *bufferPointer++ = (centerPoint.y + cosf(angle) * radiusX * aspectRatio * 0.85) * 2.0 - 1.0;
            *bufferPointer++ = -1.0;
            *bufferPointer++ = color.x;
            *bufferPointer++ = color.y;
            *bufferPointer++ = color.z;
            *bufferPointer++ = color.w;
        }
    }
    *bufferPointer++ = centerPoint.x * 2.0 - 1.0;
    *bufferPointer++ = (centerPoint.y + radiusX * aspectRatio * 0.85) * 2.0 - 1.0;
    *bufferPointer++ = -1.0;
    *bufferPointer++ = color.x;
    *bufferPointer++ = color.y;
    *bufferPointer++ = color.z;
    *bufferPointer++ = color.w;
    circle.lineBufferSize = bufferPointer - circle.lineBufferStart;
    circle.nLineVertices = circle.lineBufferSize / 7;
    return circle;
}