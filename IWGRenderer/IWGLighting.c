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
    "Light[0].OuterCutoff",
    "Light[0].InnerCutoff",
    "Light[0].Exponent",
    "Material.Ambient",
    "Material.Diffuse",
    "Material.Specular",
    "Material.Shininess",
    "Material.TextureOffset",
    "Material.TextureScale",
    "NumLight",
    "ShadingType"
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
    // Set number of lights
    // Here, fixed to one
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_NUM_LIGHT],
                1);
    // Gets or sets whether vertex lighting (Gouraud Shading) or
    // fragment lighting (Phong Shading) is used.
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADING_TYPE],
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
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_OUTER_CUTOFF],
                lightSource.OuterCutoff);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_INNER_CUTOFF],
                lightSource.InnerCutoff);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_EXPONENT],
                lightSource.Exponent);
    // Set material uniforms
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_AMBIENT],
                materialSource.Ambient.x, materialSource.Ambient.y, materialSource.Ambient.z);
    glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_DIFFUSE],
                materialSource.Diffuse.x, materialSource.Diffuse.y, materialSource.Diffuse.z, materialSource.Diffuse.w);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_SPECULAR],
                materialSource.Specular.x, materialSource.Specular.y, materialSource.Specular.z);
    glUniform1f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_SHININESS],
                materialSource.Shininess);
    glUniform2f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_TEXTURE_OFFSET],
                materialSource.TextureOffset.x, materialSource.TextureOffset.y);
    glUniform2f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_TEXTURE_SCALE],
                materialSource.TextureScale.x, materialSource.TextureScale.y);
}

IWGLightSource IWGLightingMakeBasicLight(void) {
    IWGLightSource lightSource = {
        {3.0, 2.5, 2.0},
        {0.5, 0.01, 0.0},
        {0.0, 0.0, 0.0},
        {1.0, 1.0, 1.0},
        0.0, 0.0, 0.0
    };
    return lightSource;
}

IWGMaterialSource IWGLightingMakeBasicMaterial(void) {
    IWGMaterialSource materialSource = {
        {0.1, 0.1, 0.2},
        {0.4, 0.4, 1.0, 1.0},
        {0.0, 0.0, 0.0},
        50.,
        {0.0, 0.0},
        {0.0, 0.0}
    };
    return materialSource;
}