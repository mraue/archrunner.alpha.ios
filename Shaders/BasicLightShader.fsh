#ifdef GL_ES
    precision highp float;
    precision medp int;
#endif


/// <summary>
/// Light source structure.
/// <summary>
struct LightSource
{
    vec3 Position;
    vec3 Attenuation;
    vec3 Direction;
    vec3 Color;
    float OuterCutoff;
    float InnerCutoff;
    float Exponent;
};


/// <summary>
/// Material source structure.
/// <summary>
struct MaterialSource
{
    vec3 Ambient;
    vec4 Diffuse;
    vec3 Specular;
    float Shininess;
    vec2 TextureOffset;
    vec2 TextureScale;
};


/// <summary>
/// Uniform variables.
/// <summary>
uniform int NumLight;
uniform LightSource Light[4];
uniform MaterialSource Material;

/// <summary>
/// Gets or sets whether vertex lighting (Gouraud Shading) or
/// fragment lighting (Phong Shading) is used.
/// <summary>
uniform int ShadingType;

/// <summary>
/// Varying variables.
/// <summary>
varying vec4 vWorldVertex;
varying vec3 vWorldNormal;
//varying vec2 vUv;
varying vec3 vViewVec;
varying vec4 vColor;


/// <summary>
/// Fragment shader entry.
/// <summary>
void main ()
{
    if ( ShadingType == 1 )
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
                if ( (Light[i].Direction.x != 0.0) || (Light[i].Direction.y != 0.0) || (Light[i].Direction.z != 0.0) )
                {
                    spotlight = max(-dot(lightVec, Light[i].Direction), 0.0);
                    float spotlightFade = clamp((Light[i].OuterCutoff - spotlight) / (Light[i].OuterCutoff - Light[i].InnerCutoff), 0.0, 1.0);
                    spotlight = pow(spotlight * spotlightFade, Light[i].Exponent);
                }
                
                // Calculate specular term
                vec3 r = -normalize(reflect(lightVec, normal));
                float s = pow(max(dot(r, vViewVec), 0.0), Material.Shininess);
                
                // Calculate attenuation factor
                float d = distance(vWorldVertex.xyz, Light[i].Position);
                float a = 1.0 / (Light[i].Attenuation.x + (Light[i].Attenuation.y * d) + (Light[i].Attenuation.z * d * d));
                
                // Add to color
                color += ((Material.Diffuse.xyz * l) + (Material.Specular * s)) * Light[i].Color * a * spotlight;
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