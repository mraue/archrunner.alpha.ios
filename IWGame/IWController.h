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

typedef struct {
    IWVector3 referenceDirection;
    IWVector3 direction;
    IWVector3 pitchAngleMin;
    IWVector3 pitchAngleMax;
    IWVector3 rotationSpeed;
    IWMatrix4 referenceRotationMatrix;
    IWVector3 rotationSpeedMax;
    IWVector3 debug;
} IWControllerData;

IWControllerData IWControllerDataMakeDefault();

void IWControllerDataUpdateReferenceDirection(IWControllerData *controllerData,
                                              IWVector3 newReferenceDirection,
                                              IWVector3 referenceFrame);

void IWControllerUpdateRotationSpeed(IWControllerData* cd,
                                     float deltaT);
void IWControllerAttitudeToRotationSpeed(IWControllerData *controllerData,
                                         IWVector3 attitude,
                                         bool updateZ);

IWVector3 IWControllerGetYUpForDirection(IWVector3 direction);

#endif
