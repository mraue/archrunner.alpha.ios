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

IWControllerData IWControllerDataMakeDefault()
{
    IWControllerData controllerData = {
        {0.0, 0.0, 0.0},// referenceDirection
        {0.0, 0.0, 0.0},// direction
        {1.8, 1.4, 1.8},// pitchAngleMin
        {4.4, 4.2, 4.2},// pitchAngleMax
        {0.0, 0.0, 0.0},// rotationSpeed
        IWMatrix4MakeTranslation(0.0, 0.0, 0.0),
        {80.0 / 180.0 * M_PI, 80.0 / 180.0 * M_PI, 80.0 / 180.0 * M_PI * 0.8},// rotationSpeedMax
        {0.0, 0.0, 0.0},// DEBUG
    };
    return controllerData;
}

void IWControllerDataUpdateReferenceDirection(IWControllerData *controllerData,
                                              IWVector3 newReferenceDirection,
                                              IWVector3 referenceFrame)
{
    controllerData->referenceDirection = IWVector3Normalize(newReferenceDirection);
    IWVector3 normal = IWVector3CrossProduct(controllerData->referenceDirection,
                                             referenceFrame);
    float angle = acosf(IWVector3DotProduct(controllerData->referenceDirection,
                                            referenceFrame));
    
    controllerData->referenceRotationMatrix = IWMatrix4MakeRotation(angle,
                                                                    normal.x, normal.y, normal.z);
    return;
}

IWVector3 toEuler(double x,double y,double z,double angle) {
    IWVector3 vec;
	float s=sinf(angle);
	float c=cosf(angle);
	float t=1-c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(x*x + y*y + z*z);
	// if (magnitude==0) throw error;
	// x /= magnitude;
	// y /= magnitude;
	// z /= magnitude;
	if ((x*y*t + z*s) > 0.998) { // north pole singularity detected
		vec.x = 2*atan2f(x*sinf(angle/2),cosf(angle/2));
		vec.y = M_PI/2;
		vec.z = 0;
		return vec;
	}
	if ((x*y*t + z*s) < -0.998) { // south pole singularity detected
		vec.x = -2*atan2f(x*sinf(angle/2),cosf(angle/2));
		vec.y = -M_PI/2;
		vec.z = 0;
		return vec;
	}
	vec.x = atan2f(y * s- x * z * t , 1 - (y*y+ z*z ) * t);
	vec.z = asinf(x * y * t + z * s) ;
	vec.y = -atan2f(x * s - y * z * t , 1 - (x*x + z*z) * t);
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

    IWVector3 attitude = {0.0, 0.0, 0.0};
    
    // Ok, this kinda works, but always rotates around fixed coordinate systems
    // Much better would be: rotate vector to reference vector, then calculate angles!
//    diffV.x = (atan2(cd->direction.z, cd->direction.x) - atan2(cd->referenceDirection.z, cd->referenceDirection.x)) * IW_RAD_TO_DEG;
//    diffV.y = (atan2(cd->direction.z, cd->direction.y) - atan2(cd->referenceDirection.z, cd->referenceDirection.y)) * IW_RAD_TO_DEG;
    // OK, let's try this:
//    IWVector3 normal = IWVector3CrossProduct(IWVector3Normalize(cd->referenceDirection),
//                                             IWVector3Make(0.0, 0.0, -1.0));
//    float angle = acosf(IWVector3DotProduct(IWVector3Normalize(cd->referenceDirection),
//                                            IWVector3Make(0.0, 0.0, -1.0)));
//      
//    GLKMatrix4 yRotationUpdateMatrix = GLKMatrix4MakeRotation(angle,
//                                                              normal.x, normal.y, normal.z);
    
    // This also works !!!
    IWVector3 normGLV = IWVector3Make(cd->direction.x, cd->direction.y, cd->direction.z);
    IWVector3 newVec = IWMatrix4MultiplyVector3(cd->referenceRotationMatrix, normGLV);
    //cd->debug = IWVector3Make(newVec.x, newVec.y, newVec.z);
    
    //diffV.x = atan2(newVec.z, newVec.x) * IW_RAD_TO_DEG + 90.0;
    //diffV.y = atan2(newVec.z, newVec.y) * IW_RAD_TO_DEG + 90.0;
    
    // lets try something new
    IWVector3 newNormal = IWVector3CrossProduct(newVec, IWVector3Make(0.0, 0.0, -1.0));
    float angle = IWVector3DotProduct(newVec, IWVector3Make(0.0, 0.0, -1.0));
    attitude = toEuler(newNormal.x, newNormal.y, newNormal.z, angle);
    attitude = IWVector3MultiplyScalar(attitude, 180. / M_PI);
    cd->debug = attitude;

    IWControllerAttitudeToRotationSpeed(cd, attitude, false);
    return;
}

void IWControllerAttitudeToRotationSpeed(IWControllerData *controllerData,
                                         IWVector3 attitude,
                                         bool updateZ)
{
    IWVector3 diffSigns = IWVector3Divide(attitude, IWVector3ApplyFunctionF(attitude, (float (*)(float))(fabsf)));
    
    IWVector3 rotationSpeed = IWVector3Multiply(attitude, diffSigns);
    rotationSpeed = IWVector3Maximum(IWVector3Substract(rotationSpeed, controllerData->pitchAngleMin),
                                     IWVector3Make(0.0, 0.0, 0.0));
    //IWVector3 angleMaxDeg = IWVector3Make(8.0, 4.2, 0.);
    rotationSpeed = IWVector3Divide(IWVector3Minimum(rotationSpeed,
                                                     controllerData->pitchAngleMax), controllerData->pitchAngleMax);
    rotationSpeed = IWVector3Multiply(rotationSpeed, rotationSpeed);
    rotationSpeed = IWVector3Multiply(rotationSpeed, diffSigns);
    if (isnan(rotationSpeed.x))
        rotationSpeed.x = 0.0;
    if (isnan(rotationSpeed.y))
        rotationSpeed.y = 0.0;
    if (updateZ && isnan(rotationSpeed.z))
        rotationSpeed.z = 0.0;
    if (!updateZ)
        rotationSpeed.z = controllerData->rotationSpeed.z;
    controllerData->rotationSpeed = rotationSpeed;
    return;
}

IWVector3 IWControllerGetYUpForDirection(IWVector3 direction)
{
    IWVector3 normalDirectionVec = IWVector3CrossProduct(IWVector3Make(0.0, 0.0, 1.0), direction);
    float angle = IWVector3DotProduct(IWVector3Make(0.0, 0.0, 1.0), direction);
    IWMatrix4 rotationMatrix = IWMatrix4MakeRotation(acosf(angle), normalDirectionVec.x, normalDirectionVec.y , normalDirectionVec.z);
    return IWMatrix4MultiplyVector3(rotationMatrix, IWVector3Make(0.0, 1.0, 0.0));
}