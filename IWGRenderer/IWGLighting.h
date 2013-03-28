//
//  IWGLighting.hh
//  gameoneB
//
//  Created by Martin Raue on 1/11/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWGLighting_h
#define gameoneB_IWGLighting_h

#include "IWOpenGL.h"

#include "IWMath.h"

enum {
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_POSITION,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_ATTENUATION,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_DIRECTION,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_COLOR,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_HAS_SPOT_LIGHT,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_OUTER_CUTOFF,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_INNER_CUTOFF,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_EXPONENT,
    IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_VIGNETTING_EXPONENT,
    IWGLIGHTING_UNIFORM_LOC_SUN_DIRECTION,
    IWGLIGHTING_UNIFORM_LOC_SUN_COLOR,
    IWGLIGHTING_UNIFORM_LOC_MOON_DIRECTION,
    IWGLIGHTING_UNIFORM_LOC_MOON_COLOR,
    IWGLIGHTING_UNIFORM_LOC_N
};

typedef struct
{
    IWVector3 Position;
    IWVector3 Attenuation;
    IWVector3 Direction;
    IWVector4 Color;
    int HasSpotlight;
    float OuterCutoff;
    float InnerCutoff;
    float Exponent;
    float VignettingExponent;
} IWGPointLightSourceData;

typedef struct
{
    IWVector3 Direction;
    IWVector4 Color;
} IWGBasicLightSourceData;


extern GLint IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_N];

void IWGLightingInitializeUniformLocations(GLuint program);
void IWGLightingSetUniforms(IWGBasicLightSourceData *sunLightSource,
                            IWGBasicLightSourceData *moonLightSource,
                            IWGPointLightSourceData *playerLightSource);

IWGPointLightSourceData IWGPointLightSourceMakeDefault(void);
IWGBasicLightSourceData IWGBasicLightSourceMakeDefault(void);

#endif
