//
//  IWTimer.c
//  theBr1dge
//
//  Created by Martin Raue on 1/30/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWTimer.h"

#include <stdio.h>


IWTimerData IWTimerDataMakeEmpty()
{
    IWTimerData timerData = {
        0.0, 0.0, false
    };
    return timerData;
}

IWTimerData IWTimerDataMake(float time, float duration, bool hasFinished)
{
    IWTimerData timerData = {
        time, duration, hasFinished
    };
    return timerData;
}

void IWTimerResetAndStart(IWTimerData *timer)
{
    timer->time = 0.0;
    timer->hasFinished = false;
    return;
}

bool IWTimerUpdate(IWTimerData *timer, float timeSinceLastUpdate)
{
    timer->time += timeSinceLastUpdate;
    
    if (timer->time >= timer->duration)
        timer->hasFinished = true;

    return timer->hasFinished;
}