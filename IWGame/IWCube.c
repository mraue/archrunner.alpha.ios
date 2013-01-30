//
//  IWCube.c
//  theBr1dge
//
//  Created by Martin Raue on 1/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWCube.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

IWCubeData IWCubeMake(IWCUBE_TYPE type,
                      IWVector3 centerPosition, IWVector4 color,
                      float halfLengthX, float collisionRadius,
                      bool isVisible, bool isInteractive,
                      IWVector3Transition positionTransition)
{
    IWCubeData cubeData = {
        type,
        centerPosition,
        color,
        halfLengthX,
        collisionRadius,
        isVisible, isInteractive,
        positionTransition,
        IWGPrimitiveBufferDataMakeEmpty()
    };
    cubeData.triangleBufferData.size = 6 * 6 * 10;// 6 sides * 6 vertices * 6 GLfloats (pos + norm)
    cubeData.triangleBufferData.stride = 10;
    cubeData.triangleBufferData.positionOffset = 0;
    cubeData.triangleBufferData.colorOffset = 3;
    cubeData.triangleBufferData.normalOffset = 7;
    return cubeData;
}

IWCubeData* IWCubeMakeCubeOfCube(int nx, int ny, int nz, float l, float d,
                                 IWVector4 color,
                                 unsigned int nRandomizePositions, float randomDistance)
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
                for (unsigned int i = 0; i < nRandomizePositions; i++) {
                    cubePtr->centerPosition = IWVector3Add(cubePtr->centerPosition,
                                                           IWVector3Make((IW_FRAND - 0.5) * 2.0 * randomDistance,
                                                                         (IW_FRAND - 0.5) * 2.0 * randomDistance,
                                                                         (IW_FRAND - 0.5) * 2.0 * randomDistance));
                }
                cubePtr->type = IWCUBE_TYPE_STANDARD;
                cubePtr->color = color;
                cubePtr->halfLengthX = l * 0.5;
                cubePtr->collisionRadius = l * 1.4142;
                cubePtr->isVisible = true;
                cubePtr->isInteractive = true;
                cubePtr->positionTransition = IWVector3TransitionMakeEmpty();
                cubePtr->triangleBufferData = IWGPrimitiveBufferDataMakeEmpty();
                cubePtr++;
            }
        }
    }
    return cubeOfCubeDataStart;
}

IWCubeData* IWCubeMakeCubes(int nx, int ny, int nz, float l, float d,
                            IWVector3 center, IWVector4 color,
                            unsigned int nRandomizePositions, float randomDistance)
{
    unsigned int n = nx * ny * nz;
    IWCubeData* cubeOfCubeDataStart = malloc(n * sizeof(IWCubeData));
    IWCubeData* cubePtr = cubeOfCubeDataStart;
    float dx = nx * (l + d) - d;
    float dy = ny * (l + d) - d;
    float dz = nz * (l + d) - d;
    float x0 = center.x - dx / 2.;
    float y0 = center.y - dy / 2.;
    float z0 = center.z - dz / 2.;
    for (float x = x0; x <= x0 + dx; x += d + l) {
        for (float y = y0; y < y0 + dy; y += d + l) {
            for (float z = z0; z < z0 + dz; z += d + l) {
                cubePtr->centerPosition.x = x;
                cubePtr->centerPosition.y = y;
                cubePtr->centerPosition.z = z;
                for (unsigned int i = 0; i < nRandomizePositions; i++) {
                    cubePtr->centerPosition = IWVector3Add(cubePtr->centerPosition,
                                                           IWVector3Make((IW_FRAND - 0.5) * 2.0 * randomDistance,
                                                                         (IW_FRAND - 0.5) * 2.0 * randomDistance,
                                                                         (IW_FRAND - 0.5) * 2.0 * randomDistance));
                }
                cubePtr->type = IWCUBE_TYPE_STANDARD;
                cubePtr->color = color;
                cubePtr->halfLengthX = l * 0.5;
                cubePtr->collisionRadius = l * 1.4142;
                cubePtr->isVisible = true;
                cubePtr->isInteractive = true;
                cubePtr->positionTransition = IWVector3TransitionMakeEmpty();
                cubePtr->triangleBufferData = IWGPrimitiveBufferDataMakeEmpty();
                cubePtr++;
            }
        }
    }
    return cubeOfCubeDataStart;
}

IWVector3* IWCubeMakeCubeCurve(unsigned int nPositions, IWVector3 startingPosition, IWGEOMETRY_AXIS axis)
{
    IWVector3* points = malloc(nPositions * sizeof(IWVector3));
    IWVector3* pointsStart = points;
    IWVector3* pointsEnd = points + nPositions;

    float b, boffset, br, r1, r2;
    float r1_sign1, r1_sign2, r1_sin1, r1_sin2, r1_exp;
    float r2_sign1, r2_sign2, r2_sin1, r2_sin2, r2_exp;
    float r3_atan, r3_sign;
    IWVector3 newPointOffset;
    
    unsigned int iseed = (unsigned int)time(NULL);
    srand(iseed);

    b = 0.0;
    boffset = 0.0;
    float bNextMax = 2.0 * M_PI;
    //unsigned int i = 0;

    //*points++ = startingPosition;
    while (points < pointsEnd) {
        if (b == 0.0 || b > bNextMax) {
            r1_sign1 = IW_RAND_SIGN;
            //r1_sign2 = IW_RAND_SIGN;
            r1_sin1 = IW_RAND_UNIFORM(1.0, 4.0);
            r1_sin2 = IW_RAND_UNIFORM(6.0, 10.0);
            //r1_exp = IW_RAND_UNIFORM(1.0, 4.0);
            r2_sign1 = IW_RAND_SIGN;
            //r2_sign2 = IW_RAND_SIGN;
            r2_sin1 = IW_RAND_UNIFORM(1.0, 4.0);
            r2_sin2 = IW_RAND_UNIFORM(6.0, 10.0);
            //r2_exp = IW_RAND_UNIFORM(1.0, 4.0);
            //r3_sign = IW_RAND_SIGN;
            //r3_atan = IW_RAND_UNIFORM(M_PI * 2.0, M_PI * 6.0);
            bNextMax += 2.0 * M_PI;
            //points--;
            //startingPosition = *points;
            //points++;
        }
        //br = b - boffset;
        r1 = r1_sign1 * sin(r1_sin1 * b) * cos(r1_sin2 * b) * 0.7;
        r2 = r2_sign1 * sin(r2_sin1 * b) * cos(r2_sin2 * b) * 0.7;
        switch (axis) {
            case IWGEOMETRY_AXIS_X:
                newPointOffset = IWVector3Make(b * 3.0, r1, r2);
                break;
            case IWGEOMETRY_AXIS_Y:
                newPointOffset = IWVector3Make(r1, b * 3.0, r2);
                break;
            case IWGEOMETRY_AXIS_Z:
                newPointOffset = IWVector3Make(r1, r2, b * 3.0);
                break;
        }
        *points++ = IWVector3Add(startingPosition, newPointOffset);
        b+= 2. * M_PI / 100.0;
    }

    return pointsStart;
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
    GLfloat *p = cube->triangleBufferData.startCPU;
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
    return cube->triangleBufferData.size = (p - cube->triangleBufferData.startCPU);
}