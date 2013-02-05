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

#include "IWUserInterface.h"

typedef enum {
    IWCUBE_TYPE_STANDARD,
    IWCUBE_TYPE_OVERDRIVE,
    IWCUBE_TYPE_TRANSITION,
    IWCUBE_TYPE_SPAWNING,
    IWCUBE_TYPE_POOL,
    IWCUBE_TYPE_N
} IWCUBE_TYPE;

typedef enum {
    IWCUBE_FACES_TOP  = 1 <<  0,
    IWCUBE_FACES_BOTTOM  = 1 <<  1,
    IWCUBE_FACES_LEFT = 1 <<  2,
    IWCUBE_FACES_RIGHT = 1 <<  3,
    IWCUBE_FACES_FRONT = 1 <<  4,
    IWCUBE_FACES_BACK = 1 <<  5,
    IWCUBE_FACES_ALL = (1 << 6) - 1,
    IWCUBE_FACES_LID = IWCUBE_FACES_ALL & ~IWCUBE_FACES_BOTTOM,
    IWCUBE_FACES_BOWL = IWCUBE_FACES_ALL & ~IWCUBE_FACES_TOP,
    
} IWCUBE_FACES;

typedef enum {
    IWCUBE_NORMALS_OUTWARDS,
    IWCUBE_NORMALS_INWARD
} IWCUBE_NORMALS;

struct _IWCubeData {
    unsigned int id;
    IWCUBE_TYPE type;    
    IWVector3 centerPosition;
    IWVector4 color;
    IWVector3 dimensions;
    IWCUBE_FACES faces;
    IWCUBE_NORMALS normals;
    float collisionRadius;// if == 0.0 no collision possible
    bool isVisible;
    bool isInteractive;
    IWVector3Transition positionTransition;
    IWGPrimitiveBufferData triangleBufferData;
};
typedef struct _IWCubeData IWCubeData;

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
                      IWVector3Transition positionTransition);

IWCubeData* IWCubeMakeCubes(int nx, int ny, int nz, float l, float d,
                            IWVector3 center, IWVector4 color,
                            unsigned int nRandomizePositions, float randomDistance);

IWVector3* IWCubeMakeCubeCurve(unsigned int nPositions, IWVector3 startingPosition, IWGEOMETRY_AXIS axis);

size_t IWCubeToTriangles(IWCubeData* cube);


#endif
