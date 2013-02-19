//
//  IWGLighting.c
//  gameoneB
//
//  Created by Martin Raue on 1/11/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>
#include <math.h>

#include "IWGLighting.h"

const char* IWGLIGHTING_UNIFORM_STRINGS[] = {
    "PlayerLight.Position",
    "PlayerLight.Attenuation",
    "PlayerLight.Direction",
    "PlayerLight.Color",
    "PlayerLight.HasSpotlight",    
    "PlayerLight.OuterCutoff",
    "PlayerLight.InnerCutoff",
    "PlayerLight.Exponent",
    "PlayerLight.VignettingExponent",
    "Sun.Direction",
    "Sun.Color",
    "Moon.Direction",
    "Moon.Color"
};

void IWGLightingInitializeUniformLocations(GLuint program)
{
    for (int i = 0; i < IWGLIGHTING_UNIFORM_LOC_N; i++) {
        IWGLightingUniformLocations[i] = glGetUniformLocation(program, IWGLIGHTING_UNIFORM_STRINGS[i]);
        if (IWGLightingUniformLocations[i] == -1)
            printf("Error: could not get position for uniform %s\n", IWGLIGHTING_UNIFORM_STRINGS[i]);
    }
}


void IWGLightingSetUniforms(IWGBasicLightSourceData *sunLightSource,
                            IWGBasicLightSourceData *moonLightSource,
                            IWGPointLightSourceData *playerLightSource)
{
    // Set shader type
    //glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
    //            1);
    // Gets or sets whether vertex lighting (Gouraud Shading) or
    // fragment lighting (Phong Shading) is used.
    //glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHTING_TYPE],
    //            0);
    // Set light source uniforms
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_POSITION],
                playerLightSource->Position.x, playerLightSource->Position.y, playerLightSource->Position.z);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_ATTENUATION],
                playerLightSource->Attenuation.x, playerLightSource->Attenuation.y, playerLightSource->Attenuation.z);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_DIRECTION],
                playerLightSource->Direction.x, playerLightSource->Direction.y, playerLightSource->Direction.z);
    glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_COLOR],
                playerLightSource->Color.x, playerLightSource->Color.y, playerLightSource->Color.z,
                playerLightSource->Color.w);
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_HAS_SPOT_LIGHT],
                playerLightSource->HasSpotlight);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_OUTER_CUTOFF],
                playerLightSource->OuterCutoff);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_INNER_CUTOFF],
                playerLightSource->InnerCutoff);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_EXPONENT],
                playerLightSource->Exponent);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_VIGNETTING_EXPONENT],
                playerLightSource->VignettingExponent);
    // Sun
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SUN_DIRECTION],
                sunLightSource->Direction.x, sunLightSource->Direction.y, sunLightSource->Direction.z);
    glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SUN_COLOR],
                sunLightSource->Color.x, sunLightSource->Color.y, sunLightSource->Color.z, sunLightSource->Color.w);
    // Moon
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MOON_DIRECTION],
                moonLightSource->Direction.x, moonLightSource->Direction.y, moonLightSource->Direction.z);
    glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MOON_COLOR],
                moonLightSource->Color.x, moonLightSource->Color.y, moonLightSource->Color.z, moonLightSource->Color.w);
}

IWGPointLightSourceData IWGPointLightSourceMakeDefault(void)
{
    IWGPointLightSourceData lightSource = {
        {3.0, 2.5, -2.0},// Position
        {0.5, 0.2, 0.5},// Attenuation
        {0.0, 0.0, 0.0},// Direction
        {0.8, 0.8, 0.8, 1.0},// Color
        1,// HasSpotlight
        cosf(25.0 * IW_DEG_TO_RAD), cosf(15.0 * IW_DEG_TO_RAD), 2.0,// OuterCutoff, InnerCutoff, Exponent
        7.//VignettingExponent
    };
    return lightSource;
}

IWGBasicLightSourceData IWGBasicLightSourceMakeDefault(void)
{
    IWGBasicLightSourceData lightSource = {
        {1.0, 1.0, 1.0},
        {1.0, 1.0, 1.0, 1.0}
    };
    return lightSource;
}