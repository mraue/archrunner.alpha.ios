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

enum IWRECTANGLE_ANCHOR_POSITION {
    IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
    IWRECTANGLE_ANCHOR_POSITION_UPPER_LEFT,
    IWRECTANGLE_ANCHOR_POSITION_LOWER_RIGHT,
    IWRECTANGLE_ANCHOR_POSITION_UPPER_RIGHT
};

#
# pragma mark - Function prototypes
#

IWRectangle IWRectangleMakeFromAnchorAndDimensions(IWPoint anchor, IWVector2 dimensions,
                                                   enum IWRECTANGLE_ANCHOR_POSITION anchorPosition);

static __inline__ short IWPointInRectangle(IWPoint point, IWRectangle rectangle);

#
# pragma mark - Inline implementation
#

static __inline__ short IWPointInRectangle(IWPoint point, IWRectangle rectangle)
{
    return ((point.x >= rectangle.lowerLeft.x) && (point.x < rectangle.upperRight.x)
            && (point.y >= rectangle.lowerLeft.y) && (point.y < rectangle.upperRight.y));
}

#endif
