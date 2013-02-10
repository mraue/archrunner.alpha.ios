//
//  IWVector3.h
//  gameoneB
//
//  Created by Martin Raue on 1/9/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWVector3_h
#define gameoneB_IWVector3_h

#include <math.h>

#include "IWMathTypes.h"

static __inline__ IWVector3 IWVector3Make(float x, float y, float z);
static __inline__ IWVector3 IWVector3MakeWithArray(float val[3]);

static __inline__ IWVector3 IWVector3Negate(IWVector3 vector);
                                         
static __inline__ IWVector3 IWVector3Add(IWVector3 vectorLeft, IWVector3 vectorRight);
static __inline__ IWVector3 IWVector3Substract(IWVector3 vectorLeft, IWVector3 vectorRight);
static __inline__ IWVector3 IWVector3Multiply(IWVector3 vectorLeft, IWVector3 vectorRight);
static __inline__ IWVector3 IWVector3Divide(IWVector3 vectorLeft, IWVector3 vectorRight);

static __inline__ IWVector3 IWVector3AddScalar(IWVector3 vectorLeft, float val);
static __inline__ IWVector3 IWVector3SubstractScalar(IWVector3 vectorLeft, float val);
static __inline__ IWVector3 IWVector3MultiplyScalar(IWVector3 vectorLeft, float val);
static __inline__ IWVector3 IWVector3DivideScalar(IWVector3 vectorLeft, float val);

static __inline__ IWVector3 IWVector3Maximum(IWVector3 vectorLeft, IWVector3 vectorRight);
static __inline__ IWVector3 IWVector3Minimum(IWVector3 vectorLeft, IWVector3 vectorRight);

static __inline__ IWVector3 IWVector3Normalize(IWVector3 v);

static __inline__ float IWVector3DotProduct(IWVector3 vectorLeft, IWVector3 vectorRight);
static __inline__ float IWVector3Length(IWVector3 v);
static __inline__ float IWVector3Distance(IWVector3 vStart, IWVector3 vEnd);

static __inline__ IWVector3 IWVector3Lerp(IWVector3 vectorStart, IWVector3 vectorEnd, float t);

static __inline__ IWVector3 IWVector3CrossProduct(IWVector3 vectorLeft, IWVector3 vectorRight);

#pragma mark - New functionality not in GLKVector3

static __inline__ IWVector3 IWVector3ApplyFunctionF(IWVector3 vector, float (*function)(float));
static __inline__ IWVector3 IWVector3ApplyFunctionD(IWVector3 vector, double (*function)(double));
static __inline__ float IWVector3DistanceSquared(IWVector3 vStart, IWVector3 vEnd);

#pragma mark - Implementation

static __inline__ IWVector3 IWVector3Make(float x, float y, float z) {
    IWVector3 v = {x, y, z};
    return v;
}

static __inline__ IWVector3 IWVector3MakeWithArray(float val[3]) {
    IWVector3 v = {val[0], val[1], val[2]};
    return v;
}


static __inline__ IWVector3 IWVector3Negate(IWVector3 vector) {
    IWVector3 v = {-vector.x, -vector.y, -vector.z};
    return v;
}


static __inline__ IWVector3 IWVector3Add(IWVector3 vectorLeft, IWVector3 vectorRight) {
    IWVector3 v = {vectorLeft.x + vectorRight.x, vectorLeft.y + vectorRight.y, vectorLeft.z + vectorRight.z};
    return v;
}

static __inline__ IWVector3 IWVector3Substract(IWVector3 vectorLeft, IWVector3 vectorRight) {
    IWVector3 v = {vectorLeft.x - vectorRight.x, vectorLeft.y - vectorRight.y, vectorLeft.z - vectorRight.z};
    return v;
}

static __inline__ IWVector3 IWVector3Multiply(IWVector3 vectorLeft, IWVector3 vectorRight) {
    IWVector3 v = {vectorLeft.x * vectorRight.x, vectorLeft.y * vectorRight.y, vectorLeft.z * vectorRight.z};
    return v;
}

static __inline__ IWVector3 IWVector3Divide(IWVector3 vectorLeft, IWVector3 vectorRight) {
    IWVector3 v = {vectorLeft.x / vectorRight.x, vectorLeft.y / vectorRight.y, vectorLeft.z / vectorRight.z};
    return v;
}


static __inline__ IWVector3 IWVector3AddScalar(IWVector3 vectorLeft, float val) {
    IWVector3 v = {vectorLeft.x + val, vectorLeft.y + val, vectorLeft.z + val};
    return v;
}

static __inline__ IWVector3 IWVector3SubstractScalar(IWVector3 vectorLeft, float val) {
    IWVector3 v = {vectorLeft.x - val, vectorLeft.y - val, vectorLeft.z - val};
    return v;
}

static __inline__ IWVector3 IWVector3MultiplyScalar(IWVector3 vectorLeft, float val) {
    IWVector3 v = {vectorLeft.x * val, vectorLeft.y * val, vectorLeft.z * val};
    return v;
}

static __inline__ IWVector3 IWVector3DivideScalar(IWVector3 vectorLeft, float val) {
    IWVector3 v = {vectorLeft.x / val, vectorLeft.y / val, vectorLeft.z / val};
    return v;
}

static __inline__ IWVector3 IWVector3Maximum(IWVector3 vectorLeft, IWVector3 vectorRight) {
    IWVector3 v = {
        vectorLeft.x > vectorRight.x ? vectorLeft.x : vectorRight.x,
        vectorLeft.y > vectorRight.y ? vectorLeft.y : vectorRight.y,
        vectorLeft.z > vectorRight.z ? vectorLeft.z : vectorRight.z
    };
    return v;
}

static __inline__ IWVector3 IWVector3Minimum(IWVector3 vectorLeft, IWVector3 vectorRight) {
    IWVector3 v = {
        vectorLeft.x < vectorRight.x ? vectorLeft.x : vectorRight.x,
        vectorLeft.y < vectorRight.y ? vectorLeft.y : vectorRight.y,
        vectorLeft.z < vectorRight.z ? vectorLeft.z : vectorRight.z
    };
    return v;
}

static __inline__ IWVector3 IWVector3Normalize(IWVector3 v) {
    float l = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (l != 0) {
        IWVector3 vr = {v.x / l, v.y / l, v.z / l};
        return vr;
    } else {
        IWVector3 vr = {0., 0., 0.};
        return vr;
    }
}


static __inline__ float IWVector3DotProduct(IWVector3 vectorLeft, IWVector3 vectorRight) {
    return vectorLeft.x * vectorRight.x + vectorLeft.y * vectorRight.y + vectorLeft.z * vectorRight.z;
}

static __inline__ float IWVector3Length(IWVector3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static __inline__ float IWVector3Distance(IWVector3 vStart, IWVector3 vEnd) {
    IWVector3 v = {vEnd.x - vStart.x, vEnd.y - vStart.y, vEnd.z - vStart.z};
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}


static __inline__ IWVector3 IWVector3Lerp(IWVector3 vectorStart, IWVector3 vectorEnd, float t)
{
    IWVector3 v = {
        vectorStart.x + ((vectorEnd.x - vectorStart.x) * t),
        vectorStart.y + ((vectorEnd.y - vectorStart.y) * t),
        vectorStart.z + ((vectorEnd.z - vectorStart.z) * t)
    };
    return v;
}


static __inline__ IWVector3 IWVector3CrossProduct(IWVector3 vectorLeft, IWVector3 vectorRight) {
    IWVector3 v = {
        vectorLeft.y * vectorRight.z - vectorLeft.z * vectorRight.y,
        vectorLeft.z * vectorRight.x - vectorLeft.x * vectorRight.z,
        vectorLeft.x * vectorRight.y - vectorLeft.y * vectorRight.x
    };
    return v;
}


static __inline__ IWVector3 IWVector3ApplyFunctionF(IWVector3 vector, float (*function)(float)) {
    IWVector3 newVector = {
        (*function)(vector.x),
        (*function)(vector.y),
        (*function)(vector.z)
    };
    return newVector;
}

static __inline__ IWVector3 IWVector3ApplyFunctionD(IWVector3 vector, double (*function)(double)) {
    IWVector3 newVector = {
        (*function)(vector.x),
        (*function)(vector.y),
        (*function)(vector.z)
    };
    return newVector;
}

static __inline__ float IWVector3DistanceSquared(IWVector3 vStart, IWVector3 vEnd) {
    IWVector3 v = {vEnd.x - vStart.x, vEnd.y - vStart.y, vEnd.z - vStart.z};
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

//
//static __inline__ GLKVector3 GLKVector3Negate(GLKVector3 vector);
//
//
///*
// Returns a vector whose elements are the larger of the corresponding elements of the vector arguments.
// */
//static __inline__ GLKVector3 GLKVector3Maximum(GLKVector3 vectorLeft, GLKVector3 vectorLeft);
///*
// Returns a vector whose elements are the smaller of the corresponding elements of the vector arguments.
// */
//static __inline__ GLKVector3 GLKVector3Minimum(GLKVector3 vectorLeft, GLKVector3 vectorLeft);
//
///*
// Returns true if all of the first vector's elements are equal to all of the second vector's arguments.
// */
//static __inline__ bool GLKVector3AllEqualToVector3(GLKVector3 vectorLeft, GLKVector3 vectorLeft);
///*
// Returns true if all of the vector's elements are equal to the provided value.
// */
//static __inline__ bool GLKVector3AllEqualToScalar(GLKVector3 vector, float value);
///*
// Returns true if all of the first vector's elements are greater than all of the second vector's arguments.
// */
//static __inline__ bool GLKVector3AllGreaterThanVector3(GLKVector3 vectorLeft, GLKVector3 vectorLeft);
///*
// Returns true if all of the vector's elements are greater than the provided value.
// */
//static __inline__ bool GLKVector3AllGreaterThanScalar(GLKVector3 vector, float value);
///*
// Returns true if all of the first vector's elements are greater than or equal to all of the second vector's arguments.
// */
//static __inline__ bool GLKVector3AllGreaterThanOrEqualToVector3(GLKVector3 vectorLeft, GLKVector3 vectorLeft);
///*
// Returns true if all of the vector's elements are greater than or equal to the provided value.
// */
//static __inline__ bool GLKVector3AllGreaterThanOrEqualToScalar(GLKVector3 vector, float value);
//
//
//static __inline__ GLKVector3 GLKVector3Lerp(GLKVector3 vectorStart, GLKVector3 vectorEnd, float t);
//
///*
// Project the vector, vectorToProject, onto the vector, projectionVector.
// */
//static __inline__ GLKVector3 GLKVector3Project(GLKVector3 vectorToProject, GLKVector3 projectionVector);
#endif
