//
//  IWCube.c
//  theBr1dge
//
//  Created by Martin Raue on 1/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "IWCube.h"

IWCubeData IWCubeMake(IWVector3 centerPosition, IWVector4 color,
                      float halfLengthX, float collisionRadius,
                      bool isVisible)
{
    IWCubeData cubeData = {
        centerPosition,
        color,
        halfLengthX,
        collisionRadius,
        isVisible,
        IWGPrimitiveBufferDataMakeEmpty()
    };
    cubeData.triangleBufferData.size = 6 * 6 * 10;// 6 sides * 6 vertices * 6 GLfloats (pos + norm)
    cubeData.triangleBufferData.stride = 10;
    cubeData.triangleBufferData.positionOffset = 0;
    cubeData.triangleBufferData.normalOffset = 7;
    return cubeData;
}

IWCubeData* IWCubeMakeCubeOfCube(int nx, int ny, int nz, float l, float d, IWVector4 color)
{
    unsigned int n = nx * ny * nz;
    IWCubeData* cubeOfCubeDataStart = malloc(n * sizeof(IWCubeData));
    IWCubeData* cubePtr = cubeOfCubeDataStart;
    int max = MAX(nx, MAX(ny, nz));
    float sc = 2. / (max * l  + (max + 1) * d) / l;
    float p0 = sc * (l + d);
    for (int x=0; x < nx; x++) {
        for (int y=0; y < ny; y++) {
            for (int z=0; z < nz; z++) {
                cubePtr->centerPosition.x = x * p0  - 1. + p0 - l / 2. * sc;
                cubePtr->centerPosition.y = y * p0  - 1. + p0 - l / 2. * sc;
                cubePtr->centerPosition.z = z * p0  - 1. + p0 - l / 2. * sc;
                cubePtr->color = color;
                cubePtr->halfLengthX = l;
                cubePtr->collisionRadius = l * 1.4142;
                cubePtr->isVisible = true;
                cubePtr->triangleBufferData = IWGPrimitiveBufferDataMakeEmpty();
                cubePtr++;
            }
        }
    }
    return cubeOfCubeDataStart;
}

size_t IWCubeToTriangles(IWCubeData* cube)
{
    float lh = cube->halfLengthX;
    GLfloat x = cube->centerPosition.x;
    GLfloat y = cube->centerPosition.y;
    GLfloat z = cube->centerPosition.z;
    GLfloat cr = cube->color.x;
    GLfloat cg = cube->color.y;
    GLfloat cb = cube->color.z;
    GLfloat ca = cube->color.w;
    GLfloat *p = cube->triangleBufferData.start;
    // back 1
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    // back 2
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    // top 1
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    // top 2
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    // front 1
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    // front 2
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    // bottom 1
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    // bottom 2
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    // right 1
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    // right 2
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    // left 1
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    // left 1
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    //
    return cube->triangleBufferData.size = (p - cube->triangleBufferData.start);
}