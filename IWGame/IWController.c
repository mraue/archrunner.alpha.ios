//
//  IWController.c
//  gameoneB
//
//  Created by Martin Raue on 1/9/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>
#include "math.h"

#include "IWController.h"

//IWVector3 orientationNeutral;
//IWVector3 orientation;
//IWVector3 pitchAngleMin;
//IWVector3 pitchAngleMax;
//IWVector3 rotationSpeedDeltaMax;
//IWVector3 rotationSpeed;
//IWVector3 rotationSpeedMax;

IWVector3 toEuler(double x,double y,double z,double angle) {
    IWVector3 vec;
	float s=sin(angle);
	float c=cos(angle);
	float t=1-c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(x*x + y*y + z*z);
	// if (magnitude==0) throw error;
	// x /= magnitude;
	// y /= magnitude;
	// z /= magnitude;
	if ((x*y*t + z*s) > 0.998) { // north pole singularity detected
		vec.x = 2*atan2(x*sin(angle/2),cos(angle/2));
		vec.y = M_PI/2;
		vec.z = 0;
		return vec;
	}
	if ((x*y*t + z*s) < -0.998) { // south pole singularity detected
		vec.x = -2*atan2(x*sin(angle/2),cos(angle/2));
		vec.y = -M_PI/2;
		vec.z = 0;
		return vec;
	}
	vec.x = atan2(y * s- x * z * t , 1 - (y*y+ z*z ) * t);
	vec.y = asin(x * y * t + z * s) ;
	vec.z = atan2(x * s - y * z * t , 1 - (x*x + z*z) * t);
    return vec;
}

void IWControllerUpdateRotationSpeed(IWControllerData* cd, float deltaT) {
//    // DEBUG
//    IWVector3 axis = IWVector3CrossProduct(cd->orientationNeutral, cd->orientation);
//    float angle = acos(IWVector3DotProduct(cd->orientationNeutral, cd->orientation));
//    IWVector3 angles = toEuler(axis.x, axis.y, axis.z, angle);
//    cd->rotationSpeedMax = IWVector3MultiplyScalar(angles, 180. / M_PI);
//    // END DEBUG
    // whatch out for acos if angle is large ...
//    IWVector3 diffV = IWVector3Substract(IWVector3ApplyFunctionD(cd->referenceDirection, (double (*)(double))(acos)),
//                                         IWVector3ApplyFunctionD(cd->direction, (double (*)(double))(acos)));
////    IWVector3 diffV = IWVector3Make(acos(cd->referenceDirection.x) - acos(cd->direction.x),
////                                    acos(cd->referenceDirection.y) - acos(cd->direction.y),
////                                    0.0);
//    diffV = IWVector3MultiplyScalar(diffV, IW_RAD_TO_DEG);

    IWVector3 diffV = {0.0, 0.0, 0.0};
    diffV.x = (atan2(cd->direction.z, cd->direction.x) - atan2(cd->referenceDirection.z, cd->referenceDirection.x)) * IW_RAD_TO_DEG;
    diffV.y = (atan2(cd->direction.z, cd->direction.y) - atan2(cd->referenceDirection.z, cd->referenceDirection.y)) * IW_RAD_TO_DEG;
    
//    // DEBUG
//    cd->rotationSpeedDeltaMax = IWVector3Divide(diffV, angles);
//    // END DEBUG
//    IWVector3 diffSigns = IWVector3Make(diffV.x / fabs(diffV.x),
//                                        diffV.y / fabs(diffV.y),
//                                        0.0);
    IWVector3 diffSigns = IWVector3Divide(diffV, IWVector3ApplyFunctionD(diffV, (double (*)(double))(fabs)));
    
    diffV = IWVector3Multiply(diffV, diffSigns);
    diffV = IWVector3Maximum(IWVector3Substract(diffV, cd->pitchAngleMin),
                             IWVector3Make(0.0, 0.0, 0.0));
    //IWVector3 angleMaxDeg = IWVector3Make(8.0, 4.2, 0.);
    diffV = IWVector3Divide(IWVector3Minimum(diffV, cd->pitchAngleMax), cd->pitchAngleMax);
    diffV = IWVector3Multiply(diffV, diffV);
    diffV = IWVector3Multiply(diffV, diffSigns);
    if (isnan(diffV.x))
        diffV.x = 0.0;
    if (isnan(diffV.y))
        diffV.y = 0.0;
    if (isnan(diffV.z))
        diffV.z = 0.0;
    cd->rotationSpeed = diffV;

    //IWVector3 diffV = IWVector3Substract(cd->orientation, cd->orientationNeutral);
    //cd->rotationSpeed.x = angles.x * 180. / M_PI;
//    cd->rotationSpeed.x = (atan2(cd->orientation.y, cd->orientation.x) - atan2(cd->orientationNeutral.y, cd->orientationNeutral.x))* 180. / M_PI;
//    cd->rotationSpeed.y = (atan2(cd->orientation.z, cd->orientation.y) - atan2(cd->orientationNeutral.z, cd->orientationNeutral.y))* 180. / M_PI;
//    cd->rotationSpeed.x = atan2(cd->orientation.y, cd->orientation.x) * 180. / M_PI;
//    cd->rotationSpeed.y = atan2(cd->orientation.z, cd->orientation.y) * 180. / M_PI;
    
    //cd->rotationSpeed.y = angles.y * 180. / M_PI;
//    cd->rotationSpeed.x = (acos(cd->orientationNeutral.x) - acos(cd->orientation.x))* 180. / M_PI;
//    cd->rotationSpeed.y = (acos(cd->orientationNeutral.y) - acos(cd->orientation.y))* 180. / M_PI;
//    cd->rotationSpeed.z = angles.z * 180. / M_PI;
    //float angDegX = asin(diffV.x) * 180. / 3.142;
    //cd->rotationSpeed.x = angDegX;
    //cd->rotationSpeed.x = diffV.x;
//    float sign = angDegX > 0 ? 1. : -1.;
//    angDegX = fabsf(angDegX);
//    if (angDegX > cd->pitchAngleMin.x) {
//        angDegX = angDegX > cd->pitchAngleMax.x ? cd->pitchAngleMax.x : angDegX;
//        cd->rotationSpeed.x = (angDegX - cd->pitchAngleMin.x) / cd->pitchAngleMax.x * cd->rotationSpeedMax.x;
//    } else {
//        cd->rotationSpeed.x = 0.;
//    }
//    return;
}