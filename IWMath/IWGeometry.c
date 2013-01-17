//
//  IWGeometry.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWGeometry.h"

IWRectangle IWRectangleMakeFromAnchorAndDimensions(IWPoint anchor, IWVector2 dimensions,
                                                   enum IWRECTANGLE_ANCHOR_POSITION anchorPosition)
{
    IWRectangle rectangle = {
        {0.0, 0.0},
        {0.0, 0.0}
    };
    
    switch (anchorPosition) {
            
        case IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT:
            rectangle.lowerLeft.x = anchor.x;
            rectangle.lowerLeft.y = anchor.y;
            rectangle.upperRight.x = anchor.x + dimensions.x;
            rectangle.upperRight.y = anchor.y + dimensions.y;
            break;
            
        case IWRECTANGLE_ANCHOR_POSITION_UPPER_LEFT:
            rectangle.lowerLeft.x = anchor.x;
            rectangle.lowerLeft.y = anchor.y - dimensions.y;
            rectangle.upperRight.x = anchor.x + dimensions.x;
            rectangle.upperRight.y = anchor.y;
            break;
            
        case IWRECTANGLE_ANCHOR_POSITION_LOWER_RIGHT:
            rectangle.lowerLeft.x = anchor.x - dimensions.x;
            rectangle.lowerLeft.y = anchor.y;
            rectangle.upperRight.x = anchor.x;
            rectangle.upperRight.y = anchor.y + dimensions.y;
            break;
            
        case IWRECTANGLE_ANCHOR_POSITION_UPPER_RIGHT:
            rectangle.lowerLeft.x = anchor.x - dimensions.x;
            rectangle.lowerLeft.y = anchor.y - dimensions.y;
            rectangle.upperRight.x = anchor.x;
            rectangle.upperRight.y = anchor.y;
            break;
    }
    return rectangle;
}