//
//  IWVectorTransition.c
//  theBr1dge
//
//  Created by Martin Raue on 1/28/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWVectorTransition.h"

#include <stdio.h>

IWVector4Transition IWVector4TransitionMakeEmpty(void)
{
    IWVector4Transition vector4Transition = {
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0, 0.0},
        0.0, 0.0, true, false
    };
    return vector4Transition;
}

IWVector4Transition IWVector4TransitionMake(IWVector4 startVector,
                                            IWVector4 endVector,
                                            IWVector4 currentVector,
                                            float transitionTime,
                                            float currentTransitionTime,
                                            bool transitionHasFinished,
                                            bool quadraticEaseIn)
{
    IWVector4Transition vector4Transition = {
        startVector, endVector, currentVector,
        transitionTime, currentTransitionTime,
        transitionHasFinished, quadraticEaseIn
    };
    return vector4Transition;
}

bool IWVector4TransitionUpdate(IWVector4Transition *vector4Transition, float timeSinceLastUpdate)
{
    vector4Transition->currentTransitionTime += timeSinceLastUpdate;
    
    if (vector4Transition->currentTransitionTime >= vector4Transition->transitionTime) {
        vector4Transition->currentVector = vector4Transition->endVector;
        vector4Transition->transitionHasFinished = true;
    } else {
        float lerp = vector4Transition->currentTransitionTime / vector4Transition->transitionTime;
        if (vector4Transition->quadraticEaseIn)
            lerp *= lerp;
        vector4Transition->currentVector = IWVector4Lerp(vector4Transition->startVector, vector4Transition->endVector, lerp);
        vector4Transition->transitionHasFinished = false;
    }
    return vector4Transition->transitionHasFinished;
}

void IWVector4TransitionResetAndStart(IWVector4Transition *vector4Transition)
{
    vector4Transition->transitionHasFinished = false;
    vector4Transition->currentVector = vector4Transition->startVector;
    vector4Transition->currentTransitionTime = 0.0;
    return;
}

void IWVector4TransitionReverseAndStart(IWVector4Transition *vector4Transition)
{
    vector4Transition->transitionHasFinished = false;
    vector4Transition->currentVector = vector4Transition->endVector;
    vector4Transition->endVector = vector4Transition->startVector;
    vector4Transition->startVector = vector4Transition->currentVector;
    vector4Transition->currentTransitionTime = 0.0;
    return;
}

IWVector3Transition IWVector3TransitionMakeEmpty(void)
{
    IWVector3Transition vector3Transition = {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        0.0, 0.0, true, false
    };
    return vector3Transition;
}
IWVector3Transition IWVector3TransitionMake(IWVector3 startVector,
                                            IWVector3 endVector,
                                            IWVector3 currentVector,
                                            float transitionTime,
                                            float currentTransitionTime,
                                            bool transitionHasFinished,
                                            bool quadraticEaseIn)
{
    IWVector3Transition vector3Transition = {
        startVector, endVector, currentVector,
        transitionTime, currentTransitionTime,
        transitionHasFinished, quadraticEaseIn
    };
    return vector3Transition;
}

bool IWVector3TransitionUpdate(IWVector3Transition *vector3Transition, float timeSinceLastUpdate)
{
    vector3Transition->currentTransitionTime += timeSinceLastUpdate;
    
    if (vector3Transition->currentTransitionTime >= vector3Transition->transitionTime) {
        vector3Transition->currentVector = vector3Transition->endVector;
        vector3Transition->transitionHasFinished = true;
    } else {
        float lerp = vector3Transition->currentTransitionTime / vector3Transition->transitionTime;
        if (vector3Transition->quadraticEaseIn)
            lerp *= lerp;
        vector3Transition->currentVector = IWVector3Lerp(vector3Transition->startVector, vector3Transition->endVector, lerp);
        vector3Transition->transitionHasFinished = false;
    }
    return vector3Transition->transitionHasFinished;
}

void IWVector3TransitionResetAndStart(IWVector3Transition *vector3Transition)
{
    vector3Transition->transitionHasFinished = false;
    vector3Transition->currentVector = vector3Transition->startVector;
    vector3Transition->currentTransitionTime = 0.0;
    return;
}

void IWVector3TransitionReverseAndStart(IWVector3Transition *vector3Transition)
{
    vector3Transition->transitionHasFinished = false;
    vector3Transition->currentVector = vector3Transition->endVector;
    vector3Transition->endVector = vector3Transition->startVector;
    vector3Transition->startVector = vector3Transition->currentVector;
    vector3Transition->currentTransitionTime = 0.0;
    return;
}