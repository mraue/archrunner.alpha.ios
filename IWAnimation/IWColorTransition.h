//
//  IWColorTransition.h
//  theBr1dge
//
//  Created by Martin Raue on 1/18/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWColorTransition_h
#define theBr1dge_IWColorTransition_h

#include <stdbool.h>

#include "IWMath.h"

struct _IWColorTransition {
    IWVector4 startColor;
    IWVector4 endColor;
    IWVector4 currentColor;
    float transitionTime;
    float currentTransitionTime;
    bool transitionHasFinished;
};
typedef struct _IWColorTransition IWColorTransition;

IWColorTransition IWColorTransitionMake(void);

bool IWColorTransitionUpdate(IWColorTransition *colorTransition, float timeSinceLastUpdate);

#endif
