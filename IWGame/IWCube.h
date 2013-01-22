//
//  IWCube.h
//  theBr1dge
//
//  Created by Martin Raue on 1/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWCube_h
#define theBr1dge_IWCube_h

#include <stdbool.h>

#include "IWMath.h"
#include "IWGPrimitiveBuffer.h"

struct _IWCubeData {
    IWVector3 centerPosition;
    IWVector4 color;
    float halfLengthX;
    float collisionRadius;// if == 0.0 no collision possible
    bool isVisible;
    IWGPrimitiveBufferData triangleBufferData;
};
typedef struct _IWCubeData IWCubeData;

IWCubeData IWCubeMake(IWVector3 centerPosition, IWVector4 color,
                      float halfLengthX, float collisionRadius,
                      bool isVisible);

IWCubeData* IWCubeMakeCubeOfCube(int nx, int ny, int nz, float l, float d, IWVector4 color);

size_t IWCubeToTriangles(IWCubeData* cube);


#endif
