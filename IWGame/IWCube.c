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

unsigned int IWCubeFacesToNumber(IWCUBE_FACES cubeFaces)
{
    unsigned int nFaces = 0;
    for (unsigned int i = 0; i < 6; i++) {
        nFaces += (1 <<  i) & cubeFaces ? 1 : 0;
    }
    return nFaces;
}

IWCubeData IWCubeMake(unsigned int id,
                      IWCUBE_TYPE type,
                      IWVector3 centerPosition,
                      IWVector4 color,
                      IWVector3 dimensions,
                      IWCUBE_FACES faces,
                      IWCUBE_NORMALS normals,
                      float collisionRadius,
                      bool isVisible,
                      bool isInteractive,
                      IWVector3Transition positionTransition)
{
    IWCubeData cubeData = {
        id,
        type,
        centerPosition,
        color,
        dimensions,
        faces,
        normals,
        collisionRadius,
        isVisible, isInteractive,
        positionTransition,
        IWGPrimitiveBufferDataMake(0, 10, 0, NULL, 0, 0, 0, 7, 3, 0, 0)
    };
    cubeData.triangleBufferData.size = IWCubeFacesToNumber(cubeData.faces) * 6 * 10;
    return cubeData;
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
    unsigned int id = 0;
    for (float x = x0; x <= x0 + dx; x += d + l) {
        for (float y = y0; y < y0 + dy; y += d + l) {
            for (float z = z0; z < z0 + dz; z += d + l) {
                IWVector3 centerPosition = IWVector3Make(x, y, z);
                IWVector3 dimensions =  IWVector3Make(l, l, l);
                *cubePtr = IWCubeMake(id, IWCUBE_TYPE_STANDARD, centerPosition, color, dimensions,
                                      IWCUBE_FACES_ALL, IWCUBE_NORMALS_OUTWARDS, l * 2.5, true, true,
                                      IWVector3TransitionMakeEmpty());
                for (unsigned int i = 0; i < nRandomizePositions; i++) {
                    cubePtr->centerPosition = IWVector3Add(cubePtr->centerPosition,
                                                           IWVector3Make((IW_FRAND - 0.5) * 2.0 * randomDistance,
                                                                         (IW_FRAND - 0.5) * 2.0 * randomDistance,
                                                                         (IW_FRAND - 0.5) * 2.0 * randomDistance));
                }
                cubePtr++; id++;
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

//    float b, boffset, br, r1, r2;
//    float r1_sign1, r1_sign2, r1_sin1, r1_sin2, r1_exp;
//    float r2_sign1, r2_sign2, r2_sin1, r2_sin2, r2_exp;
//    float r3_atan, r3_sign;
//    IWVector3 newPointOffset;
    float b, boffset, r1, r2;
    float r1_sign1, r1_sin1, r1_sin2;
    float r2_sign1, r2_sin1, r2_sin2;
    
    IWVector3 newPointOffset;
    
    unsigned int iseed = (unsigned int)time(NULL);
    srand(iseed);

    b = 0.0;
    boffset = 0.0;
    float bNextMax = 2.0 * M_PI;
    
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

#define IWCUBETOTRIANGLES_SET_COLOR *p++ = cr; *p++ = cg; *p++ = cb; *p++ = ca;
#define IWCUBETOTRIANGLES_SET_NORMAL(x, y, z) *p++ = x; *p++ = y; *p++ = z;

size_t IWCubeToTriangles(IWCubeData* cube)
{
    float lhx = cube->dimensions.x / 2.0;
    float lhy = cube->dimensions.y / 2.0;
    float lhz = cube->dimensions.z / 2.0;

    float normal = 1.0;
    if (cube->normals == IWCUBE_NORMALS_INWARD)
        normal *= -1.0;
    float negNormal = normal * -1.0;
    
    GLfloat x = cube->centerPosition.x;
    GLfloat y = cube->centerPosition.y;
    GLfloat z = cube->centerPosition.z;
    GLfloat cr = cube->color.x;
    GLfloat cg = cube->color.y;
    GLfloat cb = cube->color.z;
    GLfloat ca = cube->color.w;

    GLfloat *p = cube->triangleBufferData.startCPU;

    if (cube->faces & IWCUBE_FACES_BACK) {
        // back 1
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, negNormal)
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, negNormal)
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, negNormal)
        // back 2
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, negNormal)
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, negNormal)
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, negNormal)
    }

    if (cube->faces & IWCUBE_FACES_TOP) {
        // top 1
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, normal, 0.0)
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, normal, 0.0)
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, normal, 0.0)
        // top 2
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, normal, 0.0)
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, normal, 0.0)
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, normal, 0.0)
    }
    
    if (cube->faces & IWCUBE_FACES_FRONT) {
        // front 1
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, normal)
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, normal)
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, normal)
        // front 2
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, normal)
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, normal)
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, 0.0, normal)
    }
    
    if (cube->faces & IWCUBE_FACES_BOTTOM) {
        // bottom 1
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, negNormal, 0.0)
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, negNormal, 0.0)
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, negNormal, 0.0)
        // bottom 2
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, negNormal, 0.0)
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, negNormal, 0.0)
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(0.0, negNormal, 0.0)
    }
    
    if (cube->faces & IWCUBE_FACES_RIGHT) {
        // right 1
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(normal, 0.0, 0.0)
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(normal, 0.0, 0.0)
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(normal, 0.0, 0.0)
        // right 2
        *p++ = x + lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(normal, 0.0, 0.0)
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(normal, 0.0, 0.0)
        *p++ = x + lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(normal, 0.0, 0.0)
    }
    
    if (cube->faces & IWCUBE_FACES_LEFT) {
        // left 1
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(negNormal, 0.0, 0.0)
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(negNormal, 0.0, 0.0)
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(negNormal, 0.0, 0.0)
        // left 1
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z + lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(negNormal, 0.0, 0.0)
        *p++ = x - lhx; *p++ = y  - lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(negNormal, 0.0, 0.0)
        *p++ = x - lhx; *p++ = y + lhy; *p++ = z - lhz;
        IWCUBETOTRIANGLES_SET_COLOR
        IWCUBETOTRIANGLES_SET_NORMAL(negNormal, 0.0, 0.0)
    }
    //
    return cube->triangleBufferData.size = (p - cube->triangleBufferData.startCPU);
}