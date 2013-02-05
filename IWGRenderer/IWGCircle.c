//
//  IWGCircle.c
//  theBr1dge
//
//  Created by Martin Raue on 2/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGCircle.h"

#include <stdio.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

IWGCircleData IWGCircleMake(IWVector3 centerLocation,
                            IWVector3 direction,
                            IWVector4 color,
                            float radius,
                            unsigned int nTriangles)
{
    IWGCircleData circleData = {
        centerLocation,
        direction,
        color,
        radius,
        nTriangles,
        IWGPrimitiveBufferDataMake(0, 10, NULL, NULL, 0, 0, 0, 7, 3, 0, 0)
    };
    return circleData;
}

GLfloat* IWGCircleToTrianglesAddPoint(GLfloat* p, GLfloat x, GLfloat y, GLfloat z,
                                      IWVector4 color, IWVector3 normal)
{
    *p++ = x; *p++ = y; *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    *p++ = normal.x; *p++ = normal.y; *p++ = normal.z;
    return p;
}

unsigned int IWGCircleToTriangles(IWGCircleData *circle)
{
    float angleStep = 2.0 * M_PI / (circle->nTriangles - 1);
    GLfloat *p = circle->triangleBufferData.startCPU;

    if (!p) {
        printf("ERROR IWGCircleToTriangles: triangleBuffer.startCPU not set\n");
        return 0 ;
    }

    float x, y, z, xc, yc, zc, r, xn, yn;
    
    r = circle->radius;
    IWVector4 color = circle->color;
    IWVector3 normal = IWVector3Make(0.0, 0.0, 1.0);
    
    x = xc = circle->centerLocation.x;
    yc = circle->centerLocation.y;
    y = yc - r;
    z = zc = circle->centerLocation.z;
    
    for (float angle = angleStep; angle < 2.0 * M_PI + angleStep * 1.5; angle += angleStep) {
        xn = xc + sinf(angle) * r;
        yn = yc + cosf(angle) * r;
        p = IWGCircleToTrianglesAddPoint(p, xn, yn, z, color, normal);
        p = IWGCircleToTrianglesAddPoint(p, xc, yc, z, color, normal);
        p = IWGCircleToTrianglesAddPoint(p, x, y, z, color, normal);
        x = xn; y = yn;
    }

    return (p - circle->triangleBufferData.startCPU);
}