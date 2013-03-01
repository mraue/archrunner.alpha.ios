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

IWUIElementData IWUIElementMakeCircle(IWPoint2D centerPoint, float radiusX, IWVector4 color,
                                      float aspectRatio, unsigned int nDivisions, GLfloat *bufferPointer)
{
    IWUIElementData circle = {
        {{centerPoint.x - radiusX, centerPoint.y - radiusX / aspectRatio},
        {centerPoint.x + radiusX, centerPoint.y + radiusX / aspectRatio}},
        0, NULL, 0, 0, bufferPointer, 0
    };
    float angleStep = 2.0 * M_PI / (nDivisions - 1);
    bool first = true;
    // REFACTOR: This is scaffolded and needs massive refactoring
    for (float angle = 0.0; angle < 2.0 * M_PI; angle += angleStep) {
        *bufferPointer++ = (centerPoint.x + sinf(angle) * radiusX) * 2.0 - 1.0;
        *bufferPointer++ = (centerPoint.y + cosf(angle) * radiusX / aspectRatio) * 2.0 - 1.0;
        *bufferPointer++ = -1.0;
        *bufferPointer++ = color.x;
        *bufferPointer++ = color.y;
        *bufferPointer++ = color.z;
        *bufferPointer++ = color.w;
        if (first) {
            first = false;
        } else {
            *bufferPointer++ = (centerPoint.x + sinf(angle) * radiusX) * 2.0 - 1.0;
            *bufferPointer++ = (centerPoint.y + cosf(angle) * radiusX / aspectRatio) * 2.0 - 1.0;
            *bufferPointer++ = -1.0;
            *bufferPointer++ = color.x;
            *bufferPointer++ = color.y;
            *bufferPointer++ = color.z;
            *bufferPointer++ = color.w;
        }
    }
    *bufferPointer++ = centerPoint.x * 2.0 - 1.0;
    *bufferPointer++ = (centerPoint.y + radiusX / aspectRatio) * 2.0 - 1.0;
    *bufferPointer++ = -1.0;
    *bufferPointer++ = color.x;
    *bufferPointer++ = color.y;
    *bufferPointer++ = color.z;
    *bufferPointer++ = color.w;
    circle.lineBufferSize = bufferPointer - circle.lineBufferStart;
    circle.nLineVertices = circle.lineBufferSize / 7;
    return circle;
}

IWUIElementData IWUIElementMakeCubeSymbol(IWRectangle rectangle,
                                          float lidFraction,
                                          IWVector4 lightColor,
                                          IWVector4 darkColor,
                                          GLfloat *p)
{
    IWUIElementData uiElement = {
        rectangle,
        0, p, 0, 0, NULL, 0
    };
    float z = 0.0;
    float ymin = rectangle.lowerLeft.y, ymax = rectangle.upperRight.y;
    float ymid0 = ymin + (ymax - ymin) * (lidFraction / 2.0);
    float ymid1 = ymin + (ymax - ymin) * (1.0 - lidFraction);
    float ymid2 = ymin + (ymax - ymin) * (1.0 - lidFraction / 2.0);
    float xmin = rectangle.lowerLeft.x, xmax = rectangle.upperRight.x;
    float xmid = (xmin + xmax) / 2.0;
    //
    *p++ = xmin; *p++ = ymid2; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmin; *p++ = ymid0; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmid; *p++ = ymin; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    //
    *p++ = xmin; *p++ = ymid2; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmid; *p++ = ymin; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmid; *p++ = ymid1; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    //
    *p++ = xmid; *p++ = ymin; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmax; *p++ = ymid0; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmax; *p++ = ymid2; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    //
    *p++ = xmid; *p++ = ymin; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmax; *p++ = ymid2; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    *p++ = xmid; *p++ = ymid1; *p++ = z;
    *p++ = darkColor.x; *p++ = darkColor.y; *p++ = darkColor.z; *p++ = darkColor.w;
    //
    *p++ = xmin; *p++ = ymid2; *p++ = z;
    *p++ = lightColor.x; *p++ = lightColor.y; *p++ = lightColor.z; *p++ = lightColor.w;
    *p++ = xmid; *p++ = ymid1; *p++ = z;
    *p++ = lightColor.x; *p++ = lightColor.y; *p++ = lightColor.z; *p++ = lightColor.w;
    *p++ = xmax; *p++ = ymid2; *p++ = z;
    *p++ = lightColor.x; *p++ = lightColor.y; *p++ = lightColor.z; *p++ = lightColor.w;
    //
    *p++ = xmin; *p++ = ymid2; *p++ = z;
    *p++ = lightColor.x; *p++ = lightColor.y; *p++ = lightColor.z; *p++ = lightColor.w;
    *p++ = xmax; *p++ = ymid2; *p++ = z;
    *p++ = lightColor.x; *p++ = lightColor.y; *p++ = lightColor.z; *p++ = lightColor.w;
    *p++ = xmid; *p++ = ymax; *p++ = z;
    *p++ = lightColor.x; *p++ = lightColor.y; *p++ = lightColor.z; *p++ = lightColor.w;

    
    uiElement.triangleBufferSize = p - uiElement.triangleBufferStart;
    return uiElement;
}