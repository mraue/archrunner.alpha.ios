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
        IWGPrimitiveBufferDataMakeEmpty(),
        IWGPrimitiveBufferDataMakeEmpty()
    };
    return uiElement;
}

IWUIElementData IWUIElementMakeCircle(IWPoint2D centerPoint,
                                      float radiusX,
                                      IWVector4 color,
                                      float aspectRatio,
                                      unsigned int nDivisions,
                                      GLfloat *bufferPointer)
{
    return IWUIElementMakeArch(centerPoint,
                               radiusX,
                               color,
                               aspectRatio,
                               nDivisions,
                               bufferPointer,
                               0.,
                               2.0 * M_PI);
}

IWUIElementData IWUIElementMakeArch(IWPoint2D centerPoint,
                                    float radiusX,
                                    IWVector4 color,
                                    float aspectRatio,
                                    unsigned int nDivisions,
                                    GLfloat *bufferPointer,
                                    float thetaMinRad,
                                    float thetaMaxRad)
{
    IWUIElementData circle = {
        {{centerPoint.x - radiusX, centerPoint.y - radiusX / aspectRatio},
        {centerPoint.x + radiusX, centerPoint.y + radiusX / aspectRatio}},
        IWGPrimitiveBufferDataMakeEmpty(),
        IWGPrimitiveBufferDataMake(0, 7, bufferPointer, NULL, 0, 0, 0, 0, 3, 0, 0)
    };
    float angleStep = (thetaMaxRad - thetaMinRad) / (nDivisions - 1);
    for (float angle = thetaMinRad; angle < thetaMaxRad; angle += angleStep) {
        *bufferPointer++ = (centerPoint.x + sinf(angle) * radiusX) * 2.0 - 1.0;
        *bufferPointer++ = (centerPoint.y + cosf(angle) * radiusX / aspectRatio) * 2.0 - 1.0;
        *bufferPointer++ = -1.0;
        *bufferPointer++ = color.x;
        *bufferPointer++ = color.y;
        *bufferPointer++ = color.z;
        *bufferPointer++ = color.w;
        *bufferPointer++ = (centerPoint.x + sinf(angle + angleStep) * radiusX) * 2.0 - 1.0;
        *bufferPointer++ = (centerPoint.y + cosf(angle + angleStep) * radiusX / aspectRatio) * 2.0 - 1.0;
        *bufferPointer++ = -1.0;
        *bufferPointer++ = color.x;
        *bufferPointer++ = color.y;
        *bufferPointer++ = color.z;
        *bufferPointer++ = color.w;
    }
    circle.lineBuffer.size = bufferPointer - circle.lineBuffer.startCPU;
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
        IWGPrimitiveBufferDataMake(0, 7, p, NULL, 0, 0, 0, 0, 3, 0, 0),
        IWGPrimitiveBufferDataMakeEmpty()
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

    uiElement.triangleBuffer.size = p - uiElement.triangleBuffer.startCPU;
    return uiElement;
}