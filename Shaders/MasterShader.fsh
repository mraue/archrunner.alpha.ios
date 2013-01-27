#ifdef GL_ES
    precision highp float;
    precision highp int;
#endif


// Light source structure.
struct LightSource
{
    vec3 Position;
    vec3 Attenuation;
    vec3 Direction;
    vec3 Color;
    int HasSpotlight;
    float OuterCutoff;
    float InnerCutoff;
    float Exponent;
    float VignettingExponent;
};

// Material source structure.
struct MaterialSource
{
    vec3 Ambient;
    vec4 Diffuse;
};

// Uniform variables.
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

uniform int NumLight;
uniform LightSource Light[4];
uniform MaterialSource Material;

// Gets or sets whether vertex lighting (Gouraud Shading) or
// fragment lighting (Phong Shading) is used.
uniform int LightingType;

// All different shader are collected in a single master shader
// program.
//
// Possible shader types are:
//
//     0 : 2D Shader
//     1 : Standard geometry shader with dynamic lighting
//
uniform int ShaderType;

// Varying variables.
varying vec4 vWorldVertex;
varying vec3 vWorldNormal;
varying vec3 vViewVec;
varying vec4 vColor;

// Fragment shader entry.
void main ()
{
    if ( ShaderType == 0)
    {
        gl_FragColor = vColor;        
    }
    else if ( ShaderType == 1 )
    {
        if ( LightingType == 1 )
        {
            // vWorldNormal is interpolated when passed into the fragment shader.
            // We need to renormalize the vector so that it stays at unit length.
            vec3 normal = normalize(vWorldNormal);

            vec3 color = Material.Ambient;
            for (int i = 0; i < 4; ++i)
            {
                if ( i >= NumLight )
                    break;
                
                // Calculate diffuse term
                vec3 lightVec = normalize(Light[i].Position - vWorldVertex.xyz);
                //vec3 lightVec = normalize(Light[i].Position);
                float l = dot(normal, lightVec);
                if ( l > 0.0 )
                {
                    // Calculate spotlight effect
                    float spotlight = 1.0;
                    if ( Light[i].HasSpotlight != 0 )
                    {
                        spotlight = max(-dot(lightVec, Light[i].Direction), 0.0);
                        float spotlightFade = clamp((Light[i].OuterCutoff - spotlight) / (Light[i].OuterCutoff - Light[i].InnerCutoff), 0.0, 1.0);
                        spotlight = pow(spotlight * spotlightFade, Light[i].Exponent);
                    } else {
                        spotlight = max(-dot(lightVec, Light[i].Direction), 0.0);
                        spotlight = pow(spotlight, Light[i].VignettingExponent);
                    }
                    
                    // Calculate specular term
                    //vec3 r = -normalize(reflect(lightVec, normal));
                    //float s = pow(max(dot(r, vViewVec), 0.0), Material.Shininess);
                    
                    // Calculate attenuation factor
                    float d = distance(vWorldVertex.xyz, Light[i].Position);
                    float a = 1.0 / (Light[i].Attenuation.x + (Light[i].Attenuation.y * d) + (Light[i].Attenuation.z * d * d));
                    
                    // Add to color
                    //color += Material.Diffuse.xyz * l * Light[i].Color * a * spotlight;
                    color += vColor.xyz * l * Light[i].Color * a * spotlight;
                }
            }
            
            gl_FragColor = clamp(vec4(color, Material.Diffuse.w), 0.0, 1.0);
        }
        else
        {
            // Set color
            gl_FragColor = vColor;
        }
    }
    else
    {
        gl_FragColor = vColor;
    }
}