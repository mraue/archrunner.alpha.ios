//
//  IWVector2.h
//  gameoneB
//
//  Created by Martin Raue on 1/9/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWVector2_h
#define gameoneB_IWVector2_h

#include <math.h>

#include "IWMathTypes.h"

static __inline__ IWVector2 IWVector2Make(float x, float y);
static __inline__ IWVector2 IWVector2MakeWithArray(float val[2]);

static __inline__ IWVector2 IWVector2Add(IWVector2 vectorLeft, IWVector2 vectorRight);
static __inline__ IWVector2 IWVector2Substract(IWVector2 vectorLeft, IWVector2 vectorRight);
static __inline__ IWVector2 IWVector2Multiply(IWVector2 vectorLeft, IWVector2 vectorRight);
static __inline__ IWVector2 IWVector2Divide(IWVector2 vectorLeft, IWVector2 vectorRight);

static __inline__ IWVector2 IWVector2AddScalar(IWVector2 vectorLeft, float val);
static __inline__ IWVector2 IWVector2SubstractScalar(IWVector2 vectorLeft, float val);
static __inline__ IWVector2 IWVector2MultiplyScalar(IWVector2 vectorLeft, float val);
static __inline__ IWVector2 IWVector2DivideScalar(IWVector2 vectorLeft, float val);

static __inline__ IWVector2 IWVector2Maximum(IWVector2 vectorLeft, IWVector2 vectorRight);
static __inline__ IWVector2 IWVector2Minimum(IWVector2 vectorLeft, IWVector2 vectorRight);

static __inline__ IWVector2 IWVector2Normalize(IWVector2 v);

static __inline__ float IWVector2DotProduct(IWVector2 vectorLeft, IWVector2 vectorRight);
static __inline__ float IWVector2Length(IWVector2 v);
static __inline__ float IWVector2Distance(IWVector2 vStart, IWVector2 vEnd);

#pragma mark - New functionality not in GLKVector2

static __inline__ IWVector2 IWVector2ApplyFunctionF(IWVector2 vector, float (*function)(float));
static __inline__ IWVector2 IWVector2ApplyFunctionD(IWVector2 vector, double (*function)(double));

#pragma mark - Implementation

static __inline__ IWVector2 IWVector2Make(float x, float y) {
    IWVector2 v = {x, y};
    return v;
}

static __inline__ IWVector2 IWVector2MakeWithArray(float val[2]) {
    IWVector2 v = {val[0], val[1]};
    return v;
}


static __inline__ IWVector2 IWVector2Add(IWVector2 vectorLeft, IWVector2 vectorRight) {
    IWVector2 v = {vectorLeft.x + vectorRight.x, vectorLeft.y + vectorRight.y};
    return v;
}

static __inline__ IWVector2 IWVector2Substract(IWVector2 vectorLeft, IWVector2 vectorRight) {
    IWVector2 v = {vectorLeft.x - vectorRight.x, vectorLeft.y - vectorRight.y};
    return v;
}

static __inline__ IWVector2 IWVector2Multiply(IWVector2 vectorLeft, IWVector2 vectorRight) {
    IWVector2 v = {vectorLeft.x * vectorRight.x, vectorLeft.y * vectorRight.y};
    return v;
}

static __inline__ IWVector2 IWVector2Divide(IWVector2 vectorLeft, IWVector2 vectorRight) {
    IWVector2 v = {vectorLeft.x / vectorRight.x, vectorLeft.y / vectorRight.y};
    return v;
}


static __inline__ IWVector2 IWVector2AddScalar(IWVector2 vectorLeft, float val) {
    IWVector2 v = {vectorLeft.x + val, vectorLeft.y + val};
    return v;
}

static __inline__ IWVector2 IWVector2SubstractScalar(IWVector2 vectorLeft, float val) {
    IWVector2 v = {vectorLeft.x - val, vectorLeft.y - val};
    return v;
}

static __inline__ IWVector2 IWVector2MultiplyScalar(IWVector2 vectorLeft, float val) {
    IWVector2 v = {vectorLeft.x * val, vectorLeft.y * val};
    return v;
}

static __inline__ IWVector2 IWVector2DivideScalar(IWVector2 vectorLeft, float val) {
    if (val == 0.0) {
        IWVector2 v = {0.0, 0.0};
        return v;
    }
    IWVector2 v = {vectorLeft.x / val, vectorLeft.y / val};
    return v;
}

static __inline__ IWVector2 IWVector2Maximum(IWVector2 vectorLeft, IWVector2 vectorRight) {
    IWVector2 v = {
        vectorLeft.x > vectorRight.x ? vectorLeft.x : vectorRight.x,
        vectorLeft.y > vectorRight.y ? vectorLeft.y : vectorRight.y
    };
    return v;
}

static __inline__ IWVector2 IWVector2Minimum(IWVector2 vectorLeft, IWVector2 vectorRight) {
    IWVector2 v = {
        vectorLeft.x < vectorRight.x ? vectorLeft.x : vectorRight.x,
        vectorLeft.y < vectorRight.y ? vectorLeft.y : vectorRight.y
    };
    return v;
}

static __inline__ IWVector2 IWVector2Normalize(IWVector2 v) {
    float l = sqrt(v.x * v.x + v.y * v.y );
    if (l != 0) {
        IWVector2 vr = {v.x / l, v.y / l};
        return vr;
    } else {
        IWVector2 vr = {0., 0.};
        return vr;
    }
}


static __inline__ float IWVector2DotProduct(IWVector2 vectorLeft, IWVector2 vectorRight) {
    return vectorLeft.x * vectorRight.x + vectorLeft.y * vectorRight.y;
}

static __inline__ float IWVector2Length(IWVector2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

static __inline__ float IWVector2Distance(IWVector2 vStart, IWVector2 vEnd) {
    IWVector2 v = {vEnd.x - vStart.x, vEnd.y - vStart.y};
    return sqrt(v.x * v.x + v.y * v.y);
}


static __inline__ IWVector2 IWVector2ApplyFunctionF(IWVector2 vector, float (*function)(float)) {
    IWVector2 newVector = {
        (*function)(vector.x),
        (*function)(vector.y)
    };
    return newVector;
}

static __inline__ IWVector2 IWVector2ApplyFunctionD(IWVector2 vector, double (*function)(double)) {
    IWVector2 newVector = {
        (*function)(vector.x),
        (*function)(vector.y)
    };
    return newVector;
}

#endif
