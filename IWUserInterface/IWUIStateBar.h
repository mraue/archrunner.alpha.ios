//
//  IWUIStateBar.h
//  theBr1dge
//
//  Created by Martin Raue on 1/22/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIStateBar_h
#define theBr1dge_IWUIStateBar_h

#include <stdlib.h>

#include "IWGeometry.h"
#include "IWUIElement.h"

struct _IWUIStateBar {
    unsigned int nStates;
    float *states;
    IWVector4 *colors;
    IWRectangle rectangle;
    IWUI_ORIENTATION orientation;
    IWUI_DIRECTION direction;
    IWUIElement uiElementData;
};
typedef struct _IWUIStateBar IWUIStateBar;

IWUIStateBar IWUIStateBarMake(unsigned int nStates, float *states,
                              IWVector4 *colors, IWRectangle rectangle,
                              IWUI_ORIENTATION orientation, IWUI_DIRECTION direction);

size_t IWUIStateBarToTriangles(IWUIStateBar *stateBar);

#endif
