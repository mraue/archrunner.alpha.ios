//
//  IWVectorTransition.h
//  theBr1dge
//
//  Created by Martin Raue on 1/28/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWVectorTransition_h
#define theBr1dge_IWVectorTransition_h

#include <stdbool.h>

#include "IWMath.h"

typedef struct {
    IWVector4 startVector;
    IWVector4 endVector;
    IWVector4 currentVector;
    float transitionTime;
    float currentTransitionTime;
    bool transitionHasFinished;
    bool quadraticEaseIn;
} IWVector4Transition;

IWVector4Transition IWVector4TransitionMakeEmpty(void);
IWVector4Transition IWVector4TransitionMake(IWVector4 startVector,
                                            IWVector4 endVector,
                                            IWVector4 currentVector,
                                            float transitionTime,
                                            float currentTransitionTime,
                                            bool transitionHasFinished,
                                            bool quadraticEaseIn);
bool IWVector4TransitionUpdate(IWVector4Transition *vector4Transition, float timeSinceLastUpdate);
void IWVector4TransitionResetAndStart(IWVector4Transition *vector4Transition);
void IWVector4TransitionReverseAndStart(IWVector4Transition *vector4Transition);

typedef struct {
    IWVector3 startVector;
    IWVector3 endVector;
    IWVector3 currentVector;
    float transitionTime;
    float currentTransitionTime;
    bool transitionHasFinished;
    bool quadraticEaseIn;
} IWVector3Transition;

IWVector3Transition IWVector3TransitionMakeEmpty(void);
IWVector3Transition IWVector3TransitionMake(IWVector3 startVector,
                                            IWVector3 endVector,
                                            IWVector3 currentVector,
                                            float transitionTime,
                                            float currentTransitionTime,
                                            bool transitionHasFinished,
                                            bool quadraticEaseIn);
bool IWVector3TransitionUpdate(IWVector3Transition *vector3Transition, float timeSinceLastUpdate);
void IWVector3TransitionResetAndStart(IWVector3Transition *vector3Transition);


#endif
