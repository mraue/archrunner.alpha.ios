//
//  IWController.h
//  gameoneB
//
//  Created by Martin Raue on 1/9/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWController_h
#define gameoneB_IWController_h

#include "IWMath.h"

typedef struct _IWControllerData {
    IWVector3 referenceDirection;
    IWVector3 direction;
    IWVector3 pitchAngleMin;
    IWVector3 pitchAngleMax;
    IWVector3 rotationSpeed;
} IWControllerData;

void IWControllerUpdateRotationSpeed(IWControllerData* cd, float deltaT);

#endif
