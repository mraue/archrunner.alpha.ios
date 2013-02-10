//
//  IWMatrix4.h
//  theBr1dge
//
//  Created by Martin Raue on 2/10/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWMatrix4_h
#define theBr1dge_IWMatrix4_h

#include <stdbool.h>

#include "IWVector3.h"
#include "IWVector4.h"

static const IWMatrix4 IWMatrix4Identity = {
    1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0
};

/*
 m30, m31, and m32 correspond to the translation values tx, ty, tz, respectively.
 */
static __inline__ IWMatrix4 IWMatrix4Make(float m00, float m01, float m02, float m03,
                                          float m10, float m11, float m12, float m13,
                                          float m20, float m21, float m22, float m23,
                                          float m30, float m31, float m32, float m33);

static __inline__ IWMatrix4 IWMatrix4MakeTranslation(float tx, float ty, float tz);
static __inline__ IWMatrix4 IWMatrix4MakeScale(float sx, float sy, float sz);
static __inline__ IWMatrix4 IWMatrix4MakeRotation(float radians, float x, float y, float z);

static __inline__ IWMatrix4 IWMatrix4MakeXRotation(float radians);
static __inline__ IWMatrix4 IWMatrix4MakeYRotation(float radians);
static __inline__ IWMatrix4 IWMatrix4MakeZRotation(float radians);

/*
 Equivalent to gluPerspective.
 */
static __inline__ IWMatrix4 IWMatrix4MakePerspective(float fovyRadians, float aspect, float nearZ, float farZ);

/*
 Equivalent to glFrustum.
 */
static __inline__ IWMatrix4 IWMatrix4MakeFrustum(float left, float right,
                                                   float bottom, float top,
                                                   float nearZ, float farZ);

/*
 Equivalent to glOrtho.
 */
static __inline__ IWMatrix4 IWMatrix4MakeOrtho(float left, float right,
                                                 float bottom, float top,
                                                 float nearZ, float farZ);

/*
 Equivalent to gluLookAt.
 */
static __inline__ IWMatrix4 IWMatrix4MakeLookAt(float eyeX, float eyeY, float eyeZ,
                                                  float centerX, float centerY, float centerZ,
                                                  float upX, float upY, float upZ);

static __inline__ IWMatrix4 IWMatrix4Transpose(IWMatrix4 matrix);

//IWMatrix4 IWMatrix4Invert(IWMatrix4 matrix, bool *isInvertible);
//IWMatrix4 IWMatrix4InvertAndTranspose(IWMatrix4 matrix, bool *isInvertible);

static __inline__ IWMatrix4 IWMatrix4Multiply(IWMatrix4 matrixLeft, IWMatrix4 matrixRight);

static __inline__ IWMatrix4 IWMatrix4Add(IWMatrix4 matrixLeft, IWMatrix4 matrixRight);
static __inline__ IWMatrix4 IWMatrix4Subtract(IWMatrix4 matrixLeft, IWMatrix4 matrixRight);

static __inline__ IWMatrix4 IWMatrix4Translate(IWMatrix4 matrix, float tx, float ty, float tz);
static __inline__ IWMatrix4 IWMatrix4TranslateWithVector3(IWMatrix4 matrix, IWVector3 translationVector);

/*
 Assumes 0 in the w component.
 */
static __inline__ IWVector3 IWMatrix4MultiplyVector3(IWMatrix4 matrixLeft, IWVector3 vectorRight);

static __inline__ IWVector4 IWMatrix4MultiplyVector4(IWMatrix4 matrixLeft, IWVector4 vectorRight);

#pragma mark -
#pragma mark Implementations
#pragma mark -

static __inline__ IWMatrix4 IWMatrix4Make(float m00, float m01, float m02, float m03,
                                            float m10, float m11, float m12, float m13,
                                            float m20, float m21, float m22, float m23,
                                            float m30, float m31, float m32, float m33)
{
    IWMatrix4 m = {
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    };
    return m;
}


static __inline__ IWMatrix4 IWMatrix4MakeTranslation(float tx, float ty, float tz)
{
    IWMatrix4 m = IWMatrix4Identity;
    m.m30 = tx;
    m.m31 = ty;
    m.m32 = tz;
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeScale(float sx, float sy, float sz)
{
    IWMatrix4 m = IWMatrix4Identity;
    m.m00 = sx;
    m.m11 = sy;
    m.m22 = sz;
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeRotation(float radians, float x, float y, float z)
{
    IWVector3 v = IWVector3Normalize(IWVector3Make(x, y, z));
    float cos = cosf(radians);
    float cosp = 1.0f - cos;
    float sin = sinf(radians);
    
    IWMatrix4 m = { cos + cosp * v.x * v.x,
        cosp * v.x * v.y + v.z * sin,
        cosp * v.x * v.z - v.y * sin,
        0.0f,
        cosp * v.x * v.y - v.z * sin,
        cos + cosp * v.y * v.y,
        cosp * v.y * v.z + v.x * sin,
        0.0f,
        cosp * v.x * v.z + v.y * sin,
        cosp * v.y * v.z - v.x * sin,
        cos + cosp * v.z * v.z,
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeXRotation(float radians)
{
    float cos = cosf(radians);
    float sin = sinf(radians);
    
    IWMatrix4 m = { 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cos, sin, 0.0f,
        0.0f, -sin, cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeYRotation(float radians)
{
    float cos = cosf(radians);
    float sin = sinf(radians);
    
    IWMatrix4 m = { cos, 0.0f, -sin, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sin, 0.0f, cos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeZRotation(float radians)
{
    float cos = cosf(radians);
    float sin = sinf(radians);
    
    IWMatrix4 m = { cos, sin, 0.0f, 0.0f,
        -sin, cos, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakePerspective(float fovyRadians, float aspect, float nearZ, float farZ)
{
    float cotan = 1.0f / tanf(fovyRadians / 2.0f);
    
    IWMatrix4 m = { cotan / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, cotan, 0.0f, 0.0f,
        0.0f, 0.0f, (farZ + nearZ) / (nearZ - farZ), -1.0f,
        0.0f, 0.0f, (2.0f * farZ * nearZ) / (nearZ - farZ), 0.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeFrustum(float left, float right,
                                                   float bottom, float top,
                                                   float nearZ, float farZ)
{
    float ral = right + left;
    float rsl = right - left;
    float tsb = top - bottom;
    float tab = top + bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;
    
    IWMatrix4 m = { 2.0f * nearZ / rsl, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f * nearZ / tsb, 0.0f, 0.0f,
        ral / rsl, tab / tsb, -fan / fsn, -1.0f,
        0.0f, 0.0f, (-2.0f * farZ * nearZ) / fsn, 0.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeOrtho(float left, float right,
                                                 float bottom, float top,
                                                 float nearZ, float farZ)
{
    float ral = right + left;
    float rsl = right - left;
    float tab = top + bottom;
    float tsb = top - bottom;
    float fan = farZ + nearZ;
    float fsn = farZ - nearZ;
    
    IWMatrix4 m = { 2.0f / rsl, 0.0f, 0.0f, 0.0f,
        0.0f, 2.0f / tsb, 0.0f, 0.0f,
        0.0f, 0.0f, -2.0f / fsn, 0.0f,
        -ral / rsl, -tab / tsb, -fan / fsn, 1.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4MakeLookAt(float eyeX, float eyeY, float eyeZ,
                                                  float centerX, float centerY, float centerZ,
                                                  float upX, float upY, float upZ)
{
    IWVector3 ev = { eyeX, eyeY, eyeZ };
    IWVector3 cv = { centerX, centerY, centerZ };
    IWVector3 uv = { upX, upY, upZ };
    IWVector3 n = IWVector3Normalize(IWVector3Add(ev, IWVector3Negate(cv)));
    IWVector3 u = IWVector3Normalize(IWVector3CrossProduct(uv, n));
    IWVector3 v = IWVector3CrossProduct(n, u);
    
    IWMatrix4 m = { u.x, v.x, n.x, 0.0f,
        u.y, v.y, n.y, 0.0f,
        u.z, v.z, n.z, 0.0f,
        IWVector3DotProduct(IWVector3Negate(u), ev),
        IWVector3DotProduct(IWVector3Negate(v), ev),
        IWVector3DotProduct(IWVector3Negate(n), ev),
        1.0f };
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4Transpose(IWMatrix4 matrix)
{
    IWMatrix4 m = {
        matrix.m00, matrix.m10, matrix.m20, matrix.m30,
        matrix.m01, matrix.m11, matrix.m21, matrix.m31,
        matrix.m02, matrix.m12, matrix.m22, matrix.m32,
        matrix.m03, matrix.m13, matrix.m23, matrix.m33
    };
    return m;
}

static __inline__ IWMatrix4 IWMatrix4Multiply(IWMatrix4 matrixLeft, IWMatrix4 matrixRight)
{
    IWMatrix4 m;
    
    m.m00  = matrixLeft.m00 * matrixRight.m00  + matrixLeft.m10 * matrixRight.m01  + matrixLeft.m20 * matrixRight.m02   + matrixLeft.m30 * matrixRight.m03;
	m.m10  = matrixLeft.m00 * matrixRight.m10  + matrixLeft.m10 * matrixRight.m11  + matrixLeft.m20 * matrixRight.m12   + matrixLeft.m30 * matrixRight.m13;
	m.m20  = matrixLeft.m00 * matrixRight.m20  + matrixLeft.m10 * matrixRight.m21  + matrixLeft.m20 * matrixRight.m22  + matrixLeft.m30 * matrixRight.m23;
	m.m30 = matrixLeft.m00 * matrixRight.m30 + matrixLeft.m10 * matrixRight.m31 + matrixLeft.m20 * matrixRight.m32  + matrixLeft.m30 * matrixRight.m33;
    
	m.m01  = matrixLeft.m01 * matrixRight.m00  + matrixLeft.m11 * matrixRight.m01  + matrixLeft.m21 * matrixRight.m02   + matrixLeft.m31 * matrixRight.m03;
	m.m11  = matrixLeft.m01 * matrixRight.m10  + matrixLeft.m11 * matrixRight.m11  + matrixLeft.m21 * matrixRight.m12   + matrixLeft.m31 * matrixRight.m13;
	m.m21  = matrixLeft.m01 * matrixRight.m20  + matrixLeft.m11 * matrixRight.m21  + matrixLeft.m21 * matrixRight.m22  + matrixLeft.m31 * matrixRight.m23;
	m.m31 = matrixLeft.m01 * matrixRight.m30 + matrixLeft.m11 * matrixRight.m31 + matrixLeft.m21 * matrixRight.m32  + matrixLeft.m31 * matrixRight.m33;
    
	m.m02  = matrixLeft.m02 * matrixRight.m00  + matrixLeft.m12 * matrixRight.m01  + matrixLeft.m22 * matrixRight.m02  + matrixLeft.m32 * matrixRight.m03;
	m.m12  = matrixLeft.m02 * matrixRight.m10  + matrixLeft.m12 * matrixRight.m11  + matrixLeft.m22 * matrixRight.m12  + matrixLeft.m32 * matrixRight.m13;
	m.m22 = matrixLeft.m02 * matrixRight.m20  + matrixLeft.m12 * matrixRight.m21  + matrixLeft.m22 * matrixRight.m22 + matrixLeft.m32 * matrixRight.m23;
	m.m32 = matrixLeft.m02 * matrixRight.m30 + matrixLeft.m12 * matrixRight.m31 + matrixLeft.m22 * matrixRight.m32 + matrixLeft.m32 * matrixRight.m33;
    
	m.m03  = matrixLeft.m03 * matrixRight.m00  + matrixLeft.m13 * matrixRight.m01  + matrixLeft.m23 * matrixRight.m02  + matrixLeft.m33 * matrixRight.m03;
	m.m13  = matrixLeft.m03 * matrixRight.m10  + matrixLeft.m13 * matrixRight.m11  + matrixLeft.m23 * matrixRight.m12  + matrixLeft.m33 * matrixRight.m13;
	m.m23 = matrixLeft.m03 * matrixRight.m20  + matrixLeft.m13 * matrixRight.m21  + matrixLeft.m23 * matrixRight.m22 + matrixLeft.m33 * matrixRight.m23;
	m.m33 = matrixLeft.m03 * matrixRight.m30 + matrixLeft.m13 * matrixRight.m31 + matrixLeft.m23 * matrixRight.m32 + matrixLeft.m33 * matrixRight.m33;
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4Add(IWMatrix4 matrixLeft, IWMatrix4 matrixRight)
{
    IWMatrix4 m;
    
    m.m00 = matrixLeft.m00 + matrixRight.m00;
    m.m01 = matrixLeft.m01 + matrixRight.m01;
    m.m02 = matrixLeft.m02 + matrixRight.m02;
    m.m03 = matrixLeft.m03 + matrixRight.m03;
    
    m.m10 = matrixLeft.m10 + matrixRight.m10;
    m.m11 = matrixLeft.m11 + matrixRight.m11;
    m.m12 = matrixLeft.m12 + matrixRight.m12;
    m.m13 = matrixLeft.m13 + matrixRight.m13;
    
    m.m20 = matrixLeft.m20 + matrixRight.m20;
    m.m21 = matrixLeft.m21 + matrixRight.m21;
    m.m22 = matrixLeft.m22 + matrixRight.m22;
    m.m23 = matrixLeft.m23 + matrixRight.m23;
    
    m.m30 = matrixLeft.m30 + matrixRight.m30;
    m.m31 = matrixLeft.m31 + matrixRight.m31;
    m.m32 = matrixLeft.m32 + matrixRight.m32;
    m.m33 = matrixLeft.m33 + matrixRight.m33;
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4Subtract(IWMatrix4 matrixLeft, IWMatrix4 matrixRight)
{
    IWMatrix4 m;
    
    m.m00 = matrixLeft.m00 - matrixRight.m00;
    m.m01 = matrixLeft.m01 - matrixRight.m01;
    m.m02 = matrixLeft.m02 - matrixRight.m02;
    m.m03 = matrixLeft.m03 - matrixRight.m03;
    
    m.m10 = matrixLeft.m10 - matrixRight.m10;
    m.m11 = matrixLeft.m11 - matrixRight.m11;
    m.m12 = matrixLeft.m12 - matrixRight.m12;
    m.m13 = matrixLeft.m13 - matrixRight.m13;
    
    m.m20 = matrixLeft.m20 - matrixRight.m20;
    m.m21 = matrixLeft.m21 - matrixRight.m21;
    m.m22 = matrixLeft.m22 - matrixRight.m22;
    m.m23 = matrixLeft.m23 - matrixRight.m23;
    
    m.m30 = matrixLeft.m30 - matrixRight.m30;
    m.m31 = matrixLeft.m31 - matrixRight.m31;
    m.m32 = matrixLeft.m32 - matrixRight.m32;
    m.m33 = matrixLeft.m33 - matrixRight.m33;
    
    return m;
}

static __inline__ IWMatrix4 IWMatrix4Translate(IWMatrix4 matrix, float tx, float ty, float tz)
{
    IWMatrix4 m = { matrix.m00, matrix.m01, matrix.m02, matrix.m03,
        matrix.m10, matrix.m11, matrix.m12, matrix.m13,
        matrix.m20, matrix.m21, matrix.m22, matrix.m23,
        matrix.m00 * tx + matrix.m10 * ty + matrix.m20 * tz + matrix.m30,
        matrix.m01 * tx + matrix.m11 * ty + matrix.m21 * tz + matrix.m31,
        matrix.m02 * tx + matrix.m12 * ty + matrix.m22 * tz + matrix.m32,
        matrix.m33 };
    return m;
}

static __inline__ IWVector3 IWMatrix4MultiplyVector3(IWMatrix4 matrixLeft, IWVector3 vectorRight)
{
    IWVector4 v4 = IWMatrix4MultiplyVector4(matrixLeft, IWVector4Make(vectorRight.x, vectorRight.y, vectorRight.z, 0.0));
    return IWVector3Make(v4.x, v4.y, v4.z);
}

static __inline__ IWVector4 IWMatrix4MultiplyVector4(IWMatrix4 matrixLeft, IWVector4 vectorRight)
{
    IWVector4 v = { matrixLeft.m00 * vectorRight.x + matrixLeft.m10 * vectorRight.y + matrixLeft.m20 * vectorRight.z + matrixLeft.m30 * vectorRight.w,
        matrixLeft.m01 * vectorRight.x + matrixLeft.m11 * vectorRight.y + matrixLeft.m21 * vectorRight.z + matrixLeft.m31 * vectorRight.w,
        matrixLeft.m02 * vectorRight.x + matrixLeft.m12 * vectorRight.y + matrixLeft.m22 * vectorRight.z + matrixLeft.m32 * vectorRight.w,
        matrixLeft.m03 * vectorRight.x + matrixLeft.m13 * vectorRight.y + matrixLeft.m23 * vectorRight.z + matrixLeft.m33 * vectorRight.w };
    return v;
}

#endif
