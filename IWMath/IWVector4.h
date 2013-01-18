//
//  IWVector4.h
//  gameoneB
//
//  Created by Martin Raue on 1/9/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWVector4_h
#define gameoneB_IWVector4_h

#include <math.h>

#include "IWMathTypes.h"

static __inline__ IWVector4 IWVector4Make(float x, float y, float z, float w);
static __inline__ IWVector4 IWVector4MakeWithArray(float val[4]);

static __inline__ IWVector4 IWVector4Add(IWVector4 vectorLeft, IWVector4 vectorRight);
static __inline__ IWVector4 IWVector4Substract(IWVector4 vectorLeft, IWVector4 vectorRight);
static __inline__ IWVector4 IWVector4Multiply(IWVector4 vectorLeft, IWVector4 vectorRight);
static __inline__ IWVector4 IWVector4Divide(IWVector4 vectorLeft, IWVector4 vectorRight);

static __inline__ IWVector4 IWVector4AddScalar(IWVector4 vectorLeft, float val);
static __inline__ IWVector4 IWVector4SubstractScalar(IWVector4 vectorLeft, float val);
static __inline__ IWVector4 IWVector4MultiplyScalar(IWVector4 vectorLeft, float val);
static __inline__ IWVector4 IWVector4DivideScalar(IWVector4 vectorLeft, float val);

static __inline__ IWVector4 IWVector4Maximum(IWVector4 vectorLeft, IWVector4 vectorRight);
static __inline__ IWVector4 IWVector4Minimum(IWVector4 vectorLeft, IWVector4 vectorRight);

static __inline__ IWVector4 IWVector4Normalize(IWVector4 v);

static __inline__ float IWVector4DotProduct(IWVector4 vectorLeft, IWVector4 vectorRight);
static __inline__ float IWVector4Length(IWVector4 v);
static __inline__ float IWVector4Distance(IWVector4 vStart, IWVector4 vEnd);

static __inline__ IWVector4 IWVector4Lerp(IWVector4 vectorStart, IWVector4 vectorEnd, float t);

static __inline__ IWVector4 IWVector4CrossProduct(IWVector4 vectorLeft, IWVector4 vectorRight);

#pragma mark - New functionality not in GLKVector3

static __inline__ IWVector4 IWVector4ApplyFunctionF(IWVector4 vector, float (*function)(float));
static __inline__ IWVector4 IWVector4ApplyFunctionD(IWVector4 vector, double (*function)(double));

#pragma mark - Implementation

static __inline__ IWVector4 IWVector4Make(float x, float y, float z, float w)
{
    IWVector4 v = {x, y, z, w};
    return v;
}

static __inline__ IWVector4 IWVector4MakeWithArray(float val[3])
{
    IWVector4 v = {val[0], val[1], val[2], val[3]};
    return v;
}


static __inline__ IWVector4 IWVector4Add(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    IWVector4 v = {
        vectorLeft.x + vectorRight.x, vectorLeft.y + vectorRight.y,
        vectorLeft.z + vectorRight.z, vectorLeft.w + vectorRight.w
    };
    return v;
}

static __inline__ IWVector4 IWVector4Substract(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    IWVector4 v = {
        vectorLeft.x - vectorRight.x, vectorLeft.y - vectorRight.y,
        vectorLeft.z - vectorRight.z, vectorLeft.w - vectorRight.w
    };
    return v;
}

static __inline__ IWVector4 IWVector4Multiply(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    IWVector4 v = {
        vectorLeft.x * vectorRight.x, vectorLeft.y * vectorRight.y,
        vectorLeft.z * vectorRight.z, vectorLeft.w * vectorRight.w
    };
    return v;
}

static __inline__ IWVector4 IWVector4Divide(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    IWVector4 v = {
        vectorLeft.x / vectorRight.x, vectorLeft.y / vectorRight.y,
        vectorLeft.z / vectorRight.z, vectorLeft.w / vectorRight.w
    };
    return v;
}


static __inline__ IWVector4 IWVector4AddScalar(IWVector4 vectorLeft, float val)
{
    IWVector4 v = {vectorLeft.x + val, vectorLeft.y + val, vectorLeft.z + val, vectorLeft.w + val};
    return v;
}

static __inline__ IWVector4 IWVector4SubstractScalar(IWVector4 vectorLeft, float val)
{
    IWVector4 v = {vectorLeft.x - val, vectorLeft.y - val, vectorLeft.z - val, vectorLeft.w - val};
    return v;
}

static __inline__ IWVector4 IWVector4MultiplyScalar(IWVector4 vectorLeft, float val)
{
    IWVector4 v = {vectorLeft.x * val, vectorLeft.y * val, vectorLeft.z * val, vectorLeft.w * val};
    return v;
}

static __inline__ IWVector4 IWVector4DivideScalar(IWVector4 vectorLeft, float val)
{
    IWVector4 v = {vectorLeft.x / val, vectorLeft.y / val, vectorLeft.z / val, vectorLeft.w / val};
    return v;
}

static __inline__ IWVector4 IWVector4Maximum(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    IWVector4 v = {
        vectorLeft.x > vectorRight.x ? vectorLeft.x : vectorRight.x,
        vectorLeft.y > vectorRight.y ? vectorLeft.y : vectorRight.y,
        vectorLeft.z > vectorRight.z ? vectorLeft.z : vectorRight.z,
        vectorLeft.w > vectorRight.w ? vectorLeft.w : vectorRight.w
    };
    return v;
}

static __inline__ IWVector4 IWVector4Minimum(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    IWVector4 v = {
        vectorLeft.x < vectorRight.x ? vectorLeft.x : vectorRight.x,
        vectorLeft.y < vectorRight.y ? vectorLeft.y : vectorRight.y,
        vectorLeft.z < vectorRight.z ? vectorLeft.z : vectorRight.z,
        vectorLeft.w < vectorRight.w ? vectorLeft.w : vectorRight.w
    };
    return v;
}

static __inline__ IWVector4 IWVector4Normalize(IWVector4 v)
{
    float l = sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w *v.w);
    if (l != 0) {
        IWVector4 vr = {v.x / l, v.y / l, v.z / l, v.w / l};
        return vr;
    } else {
        IWVector4 vr = {0., 0., 0., 0.};
        return vr;
    }
}


static __inline__ float IWVector4DotProduct(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    return (vectorLeft.x * vectorRight.x + vectorLeft.y * vectorRight.y
            + vectorLeft.z * vectorRight.z + vectorLeft.w * vectorRight.w);
}

static __inline__ float IWVector4Length(IWVector4 v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

static __inline__ float IWVector4Distance(IWVector4 vStart, IWVector4 vEnd)
{
    IWVector4 v = {vEnd.x - vStart.x, vEnd.y - vStart.y, vEnd.z - vStart.z, vEnd.w - vStart.w};
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}


static __inline__ IWVector4 IWVector4Lerp(IWVector4 vectorStart, IWVector4 vectorEnd, float t)
{
    IWVector4 v = {
        vectorStart.x + ((vectorEnd.x - vectorStart.x) * t),
        vectorStart.y + ((vectorEnd.y - vectorStart.y) * t),
        vectorStart.z + ((vectorEnd.z - vectorStart.z) * t),
        vectorStart.w + ((vectorEnd.w - vectorStart.w) * t)
    };
    return v;
}


static __inline__ IWVector4 IWVector4CrossProduct(IWVector4 vectorLeft, IWVector4 vectorRight)
{
    IWVector4 v = {
        vectorLeft.y * vectorRight.z - vectorLeft.z * vectorRight.y,
        vectorLeft.z * vectorRight.x - vectorLeft.x * vectorRight.z,
        vectorLeft.x * vectorRight.y - vectorLeft.y * vectorRight.x,
        0.0
    };
    return v;
}


static __inline__ IWVector4 IWVector4ApplyFunctionF(IWVector4 vector, float (*function)(float))
{
    IWVector4 newVector = {
        (*function)(vector.x),
        (*function)(vector.y),
        (*function)(vector.z),
        (*function)(vector.w)
    };
    return newVector;
}

static __inline__ IWVector4 IWVector4ApplyFunctionD(IWVector4 vector, double (*function)(double))
{
    IWVector4 newVector = {
        (*function)(vector.x),
        (*function)(vector.y),
        (*function)(vector.z),
        (*function)(vector.w)
    };
    return newVector;
}

//
//static __inline__ GLKVector3 GLKVector3Negate(GLKVector3 vector);
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
///*
// Project the vector, vectorToProject, onto the vector, projectionVector.
// */
//static __inline__ GLKVector3 GLKVector3Project(GLKVector3 vectorToProject, GLKVector3 projectionVector);
#endif
