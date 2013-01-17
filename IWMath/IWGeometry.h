//
//  IWGeometry.h
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGeometry_h
#define theBr1dge_IWGeometry_h

#include "IWMath.h"

#
# pragma mark - Data Types
#

typedef IWVector2 IWPoint;

struct _IWRectangle {
    IWPoint lowerLeft;
    IWPoint upperRight;
};

typedef struct _IWRectangle IWRectangle;

#
# pragma mark - Function prototypes
#

static __inline__ short IWPointInRectangle(IWPoint point, IWRectangle rectangle);

#
# pragma mark - Implementation
#

static __inline__ short IWPointInRectangle(IWPoint point, IWRectangle rectangle)
{
    return ((point.x >= rectangle.lowerLeft.x) && (point.x < rectangle.upperRight.x)
            && (point.y >= rectangle.lowerLeft.y) && (point.y < rectangle.upperRight.y));
}

#endif
