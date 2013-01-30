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
#include "IWGeometry.h"
#include "IWVectorTransition.h"
#include "IWGPrimitiveBuffer.h"

typedef enum {
    IWCUBE_TYPE_STANDARD,
    IWCUBE_TYPE_OVERDRIVE,
    IWCUBE_TYPE_TRANSITION,
    IWCUBE_TYPE_N
} IWCUBE_TYPE;

struct _IWCubeData {
    IWCUBE_TYPE type;    
    IWVector3 centerPosition;
    IWVector4 color;
    float halfLengthX;
    float collisionRadius;// if == 0.0 no collision possible
    bool isVisible;
    bool isInteractive;
    IWVector3Transition positionTransition;
    IWGPrimitiveBufferData triangleBufferData;
};
typedef struct _IWCubeData IWCubeData;

IWCubeData IWCubeMake(IWCUBE_TYPE type,
                      IWVector3 centerPosition, IWVector4 color,
                      float halfLengthX, float collisionRadius,
                      bool isVisible, bool isInteractive,
                      IWVector3Transition positionTransition);

IWCubeData* IWCubeMakeCubeOfCube(int nx, int ny, int nz, float l, float d,
                                 IWVector4 color,
                                 unsigned int nRandomizePositions, float randomDistance);

IWCubeData* IWCubeMakeCubes(int nx, int ny, int nz, float l, float d,
                            IWVector3 center, IWVector4 color,
                            unsigned int nRandomizePositions, float randomDistance);

IWVector3* IWCubeMakeCubeCurve(unsigned int nPositions, IWVector3 startingPosition, IWGEOMETRY_AXIS axis);

size_t IWCubeToTriangles(IWCubeData* cube);


#endif
