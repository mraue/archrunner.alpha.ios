//
//  IWCubeCounter.h
//  theBr1dge
//
//  Created by Martin Raue on 2/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWCubeCounter_h
#define theBr1dge_IWCubeCounter_h

#include "IWMath.h"
#include "IWUserInterface.h"

typedef struct {
    unsigned int total;
    unsigned int spawned;
    unsigned int bridge;
    unsigned int pool;
    IWUIStateBar stateBar;
} IWCubeCounterData;

IWCubeCounterData IWCubeCounterMake(unsigned int total);

void IWCubeCounterUpdateStateBar(IWCubeCounterData *cubeCounter);

#endif
