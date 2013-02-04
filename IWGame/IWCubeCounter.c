//
//  IWCubeCounter.c
//  theBr1dge
//
//  Created by Martin Raue on 2/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWCubeCounter.h"

#include <stdio.h>

#include "IWGeometry.h"

IWCubeCounterData IWCubeCounterMake(unsigned int total)
{
    float states[] = {1.0, 0.0, 0.0};
    IWVector4 colors[] = {IWUI_COLOR_BLUE(0.6), IWUI_COLOR_GOLD(0.6), {0.6, 0.6, 0.6, 0.6}};
    IWCubeCounterData cubeCounter = {
        total, total, 0, 0,
        IWUIStateBarMake(3, states, colors, IWRectangleMake(0.98, 0.6, 0.994, 0.99),
                         IWUI_ORIENTATION_VERTICAL, IWUI_DIRECTION_REVERSE)
    };
    cubeCounter.stateBar.triangleBufferData.stride = 10;
    cubeCounter.stateBar.triangleBufferData.size = cubeCounter.stateBar.triangleBufferData.stride * 6 * 4;
    return cubeCounter;
}

void IWCubeCounterUpdateStateBar(IWCubeCounterData *cubeCounter)
{
    cubeCounter->stateBar.states[0] = (float)cubeCounter->spawned / cubeCounter->total;
    cubeCounter->stateBar.states[1] = (float)cubeCounter->bridge / cubeCounter->total + cubeCounter->stateBar.states[0];
    cubeCounter->stateBar.states[2] = (float)cubeCounter->pool / cubeCounter->total + cubeCounter->stateBar.states[1];
}