//
//  IWGLighting.hh
//  gameoneB
//
//  Created by Martin Raue on 1/11/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWGLighting_h
#define gameoneB_IWGLighting_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

enum {
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_POSITION,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_ATTENUATION,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_DIRECTION,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_COLOUR,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_HAS_SPOT_LIGHT,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_OUTER_CUTOFF,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_INNER_CUTOFF,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_EXPONENT,
    IWGLIGHTING_UNIFORM_LOC_LIGHT0_VIGNETTING_EXPONENT,
    IWGLIGHTING_UNIFORM_LOC_MATERIAL_AMBIENT,
    IWGLIGHTING_UNIFORM_LOC_MATERIAL_DIFFUSE,
    IWGLIGHTING_UNIFORM_LOC_NUM_LIGHT,
    IWGLIGHTING_UNIFORM_LOC_LIGHTING_TYPE,
    IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE,
    IWGLIGHTING_UNIFORM_LOC_N
};

// Light source structure.
struct _IWGLightSource
{
    IWVector3 Position;
    IWVector3 Attenuation;
    IWVector3 Direction;
    IWVector3 Colour;
    int HasSpotlight;
    float OuterCutoff;
    float InnerCutoff;
    float Exponent;
    float VignettingExponent;
};
typedef struct _IWGLightSource IWGLightSource;

// Material source structure.
struct _IWGMaterialSource
{
    IWVector3 Ambient;
    IWVector4 Diffuse;
};
typedef struct _IWGMaterialSource IWGMaterialSource;

GLint IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_N];

void IWGLightingInitializeUniformLocations(GLuint program);
void IWGLightingSetUniforms(IWGLightSource lightSource, IWGMaterialSource materialSource);

IWGLightSource IWGLightingMakeBasicLight(void);
IWGMaterialSource IWGLightingMakeBasicMaterial(void);

#endif
