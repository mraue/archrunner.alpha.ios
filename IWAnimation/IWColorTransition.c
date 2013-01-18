//
//  IWColorTransition.c
//  theBr1dge
//
//  Created by Martin Raue on 1/18/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWColorTransition.h"

IWColorTransition IWColorTransitionMake(void)
{
    IWColorTransition colorTransition = {
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        0.0, 0.0, true, false
    };
    return colorTransition;
}

bool IWColorTransitionUpdate(IWColorTransition *colorTransition, float timeSinceLastUpdate)
{
    colorTransition->currentTransitionTime += timeSinceLastUpdate;

    if (colorTransition->currentTransitionTime >= colorTransition->transitionTime) {
        colorTransition->currentColor = colorTransition->endColor;
        colorTransition->transitionHasFinished = true;
    } else {
        float lerp = colorTransition->currentTransitionTime / colorTransition->transitionTime;
        if (colorTransition->quadraticEaseIn)
            lerp *= lerp;
        colorTransition->currentColor = IWVector4Lerp(colorTransition->startColor, colorTransition->endColor, lerp);
        colorTransition->transitionHasFinished = false;
    }
    return colorTransition->transitionHasFinished;
}