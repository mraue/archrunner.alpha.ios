//
//  IWMathTypes.h
//  gameoneB
//
//  Created by Martin Raue on 1/8/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWMathTypes_h
#define gameoneB_IWMathTypes_h

struct _IWVector2 {
    float x;
    float y;
};

typedef struct _IWVector2 IWVector2;

struct _IWVector3 {
    float x;
    float y;
    float z;
};

typedef struct _IWVector3 IWVector3;

struct _IWVector4 {
    float x;
    float y;
    float z;
    float w;
};

typedef struct _IWVector4 IWVector4;

//struct _IWMatrix3 {
//    float m00, m01, m02;
//    float m10, m11, m12;
//    float m20, m21, m22;
//};
//typedef struct _IWMatrix3 IWMatrix3;

union _IWMatrix3
{
    struct
    {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;
    };
    float m[9];
};
typedef union _IWMatrix3 IWMatrix3;

//struct _IWMatrix4 {
//    float m00, m01, m02, m03;
//    float m10, m11, m12, m13;
//    float m20, m21, m22, m23;
//    float m30, m31, m32, m33;
//};
//
//typedef struct _IWMatrix4 IWMatrix4;

union _IWMatrix4
{
    struct
    {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    float m[16];
} __attribute__((aligned(16)));
typedef union _IWMatrix4 IWMatrix4;

#endif
