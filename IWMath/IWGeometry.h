//
//  IWGeometry.h
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGeometry_h
#define theBr1dge_IWGeometry_h

#include <stdbool.h>

#include "IWMath.h"

# pragma mark - Data Types

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

typedef enum {
    IWGEOMETRY_AXIS_X,
    IWGEOMETRY_AXIS_Y,
    IWGEOMETRY_AXIS_Z
} IWGEOMETRY_AXIS;

# pragma mark - Function prototypes

static __inline__ IWRectangle IWRectangleMake(float x1, float y1, float x2, float y2)
{
    IWRectangle rectangle = {{x1, y1}, {x2, y2}};
    return rectangle;
}

IWRectangle IWRectangleMakeFromAnchorAndDimensions(IWPoint anchor, IWVector2 dimensions,
                                                   enum IWRECTANGLE_ANCHOR_POSITION anchorPosition);

static __inline__ bool IWPointInRectangle(IWPoint point, IWRectangle rectangle);

# pragma mark - Inline implementation

static __inline__ bool IWPointInRectangle(IWPoint point, IWRectangle rectangle)
{
    return ((point.x >= rectangle.lowerLeft.x) && (point.x < rectangle.upperRight.x)
            && (point.y >= rectangle.lowerLeft.y) && (point.y < rectangle.upperRight.y));
}

#endif
