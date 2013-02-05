//
//  IWGCircle.h
//  theBr1dge
//
//  Created by Martin Raue on 2/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGCircle_h
#define theBr1dge_IWGCircle_h

#include "IWMath.h"

#include "IWGPrimitiveBuffer.h"

typedef struct {
    IWVector3 centerLocation;
    IWVector3 direction;
    IWVector4 color;
    float radius;
    unsigned int nTriangles;
    IWGPrimitiveBufferData triangleBufferData;
} IWGCircleData;

IWGCircleData IWGCircleMake(IWVector3 centerLocation,
                            IWVector3 direction,
                            IWVector4 color,
                            float radius,
                            unsigned int nTriangles);

unsigned int IWGCircleToTriangles(IWGCircleData *circle);

#endif
