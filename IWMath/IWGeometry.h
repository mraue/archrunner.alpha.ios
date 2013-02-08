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

typedef IWVector2 IWPoint2D;

typedef struct {
    IWPoint2D lowerLeft;
    IWPoint2D upperRight;
} IWRectangle;

typedef enum {
    IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
    IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
    IWGEOMETRY_ANCHOR_POSITION_LOWER_RIGHT,
    IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT
} IWGEOMETRY_ANCHOR_POSITION;

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

IWRectangle IWRectangleMakeFromAnchorAndDimensions(IWPoint2D anchor, IWVector2 dimensions,
                                                   IWGEOMETRY_ANCHOR_POSITION anchorPosition);

static __inline__ bool IWPointInRectangle(IWPoint2D point, IWRectangle rectangle);

# pragma mark - Inline implementation

static __inline__ bool IWPointInRectangle(IWPoint2D point, IWRectangle rectangle)
{
    return ((point.x >= rectangle.lowerLeft.x) && (point.x < rectangle.upperRight.x)
            && (point.y >= rectangle.lowerLeft.y) && (point.y < rectangle.upperRight.y));
}

#endif
