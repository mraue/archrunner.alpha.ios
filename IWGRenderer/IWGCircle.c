//
//  IWGCircle.c
//  theBr1dge
//
//  Created by Martin Raue on 2/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGCircle.h"

#include <stdio.h>

#include "IWOpenGL.h"

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
        IWGPrimitiveBufferDataMake(nTriangles * 3 * 10, 10, NULL, NULL, 0, 0, 0, 7, 3, 0, 0)
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

    float x, y, z, xc, yc, zc, r, xn, yn, zn, xr, yr;
    
    r = circle->radius;
    IWVector4 color = circle->color;
    IWVector3 normal = IWVector3Make(0.0, 0.0, 1.0);
    
    IWVector3 normalDirectionVec = IWVector3CrossProduct(IWVector3Make(0.0, 0.0, 1.0), circle->direction);
    float angle = IWVector3DotProduct(IWVector3Make(0.0, 0.0, 1.0), circle->direction);
    IWMatrix4 rotationMatrix = IWMatrix4MakeRotation(acosf(angle), normalDirectionVec.x, normalDirectionVec.y , normalDirectionVec.z);
    IWVector3 newPos, newR;
    
    x = xc = circle->centerLocation.x;
    yc = circle->centerLocation.y;
    y = yc - r;
    z = zc = circle->centerLocation.z;
    
    newPos = IWMatrix4MultiplyVector3(rotationMatrix, IWVector3Make(x, y, z));
    
    newR = IWMatrix4MultiplyVector3(rotationMatrix, IWVector3Make(0.0, -r, 0.0));
    x = xc + newR.x;
    y = yc + newR.y;
    z = zc + newR.z;
    
    for (float angle = angleStep; angle < 2.0 * M_PI + angleStep * 1.5; angle += angleStep) {
        xr = sinf(angle) * r;
        yr = cosf(angle) * r;
        newR = IWMatrix4MultiplyVector3(rotationMatrix, IWVector3Make(xr, yr, 0.0));
        xn = xc + newR.x;
        yn = yc + newR.y;
        zn = zc + newR.z;
        p = IWGCircleToTrianglesAddPoint(p, xn, yn, zn, color, normal);
        p = IWGCircleToTrianglesAddPoint(p, xc, yc, zc, color, normal);
        p = IWGCircleToTrianglesAddPoint(p, x, y, z, color, normal);
        x = xn; y = yn; z = zn;
    }

    return (p - circle->triangleBufferData.startCPU);
}