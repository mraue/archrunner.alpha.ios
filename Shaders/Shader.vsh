//
//  Shader.vsh
//  gameone
//
//  Created by Martin Raue on 1/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

attribute vec4 position;
attribute vec3 normal;

varying lowp vec4 colorVarying;
varying lowp float intensityVarying;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

uniform mat3 normalMatrix;
uniform vec4 lightDiffuseColor;

void main()
{
    vec3 eyeNormal = normalize(normalMatrix * normal);
    vec3 lightPosition = vec3(0.5, 1.0, 1.0);
    //vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);

    float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));

    colorVarying = lightDiffuseColor * nDotVP;
    intensityVarying = nDotVP;

    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * position;

    //gl_Position = modelViewProjectionMatrix * position;
}
