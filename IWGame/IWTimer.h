//
//  IWTimer.h
//  theBr1dge
//
//  Created by Martin Raue on 1/30/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWTimer_h
#define theBr1dge_IWTimer_h

#include <stdbool.h>

typedef struct {
    float time;
    float duration;
    bool hasFinished;
} IWTimerData;

IWTimerData IWTimerDataMakeEmpty();
IWTimerData IWTimerDataMake(float time, float duration, bool hasFinished);

void IWTimerResetAndStart(IWTimerData *timer);
bool IWTimerUpdate(IWTimerData *timer, float timeSinceLastUpdate);

#endif
