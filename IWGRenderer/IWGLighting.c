//
//  IWGLighting.c
//  gameoneB
//
//  Created by Martin Raue on 1/11/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWGLighting.h"

const char* IWGLIGHTING_UNIFORM_STRINGS[] = {
    "Light[0].Position",
    "Light[0].Attenuation",
    "Light[0].Direction",
    "Light[0].Colour",
    "Light[0].HasSpotlight",    
    "Light[0].OuterCutoff",
    "Light[0].InnerCutoff",
    "Light[0].Exponent",
    "Light[0].VignettingExponent",    
    "Material.Ambient",
    "Material.Diffuse",
    "NumLight",
    "LightingType",
    "ShaderType"
};

void IWGLightingInitializeUniformLocations(GLuint program)
{
    for (int i = 0; i < IWGLIGHTING_UNIFORM_LOC_N; i++) {
        IWGLightingUniformLocations[i] = glGetUniformLocation(program, IWGLIGHTING_UNIFORM_STRINGS[i]);
        if (IWGLightingUniformLocations[i] == 0)
            printf("Error: could not get position for uniform %s", IWGLIGHTING_UNIFORM_STRINGS[i]);
    }
}

void IWGLightingSetUniforms(IWGLightSource lightSource,
                            IWGMaterialSource materialSource)
{
    // Set shader type
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
                1);
    // Set number of lights
    // Here, fixed to one
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_NUM_LIGHT],
                1);
    // Gets or sets whether vertex lighting (Gouraud Shading) or
    // fragment lighting (Phong Shading) is used.
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHTING_TYPE],
                0);
    // Set light source uniforms
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_POSITION],
                lightSource.Position.x, lightSource.Position.y, lightSource.Position.z);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_ATTENUATION],
                lightSource.Attenuation.x, lightSource.Attenuation.y, lightSource.Attenuation.z);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_DIRECTION],
                lightSource.Direction.x, lightSource.Direction.y, lightSource.Direction.z);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_COLOUR],
                lightSource.Colour.x, lightSource.Colour.y, lightSource.Colour.z);
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_HAS_SPOT_LIGHT],
                lightSource.HasSpotlight);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_OUTER_CUTOFF],
                lightSource.OuterCutoff);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_INNER_CUTOFF],
                lightSource.InnerCutoff);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_EXPONENT],
                lightSource.Exponent);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_VIGNETTING_EXPONENT],
                lightSource.VignettingExponent);
    // Set material uniforms
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_AMBIENT],
                materialSource.Ambient.x, materialSource.Ambient.y, materialSource.Ambient.z);
    glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_DIFFUSE],
                materialSource.Diffuse.x, materialSource.Diffuse.y, materialSource.Diffuse.z, materialSource.Diffuse.w);
}

IWGLightSource IWGLightingMakeBasicLight(void) {
    IWGLightSource lightSource = {
        {3.0, 2.5, 2.0},// Position
        {0.5, 0.1, 1.0},// Attenuation
        {0.0, 0.0, 0.0},// Direction
        {1.0, 1.0, 1.0},// Colour
        0,// HasSpotlight
        0.0, 0.0, 0.0,// OuterCutoff, InnerCutoff, Exponent
        7.//VignettingExponent
    };
    return lightSource;
}

IWGMaterialSource IWGLightingMakeBasicMaterial(void) {
    IWGMaterialSource materialSource = {
        {0.0, 0.0, 0.0},// Ambient
        {0.4, 0.4, 1.0, 1.0}// Diffuse
    };
    return materialSource;
}