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
/// Attributes.
/// <summary>
attribute vec3 Vertex;
//attribute vec2 Uv;
attribute vec3 Normal;


/// <summary>
/// Uniform variables.
/// <summary>
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
//uniform vec3 ModelScale;
uniform mat3 NormalMatrix;

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
/// Vertex shader entry.
/// <summary>
void main ()
{
    //vWorldVertex = ModelMatrix * vec4(Vertex * ModelScale, 1.0);
    vWorldVertex = ModelMatrix * vec4(Vertex, 1.0);
    vec4 viewVertex = ViewMatrix * vWorldVertex;
    gl_Position = ProjectionMatrix * viewVertex;
    
    //vUv = Material.TextureOffset + (Uv * Material.TextureScale);
    
    // Original
    //vWorldNormal = normalize(mat3(ModelMatrix) * Normal);
    // Improved !!!
    vWorldNormal = normalize(mat3(NormalMatrix) * Normal);
    
    vViewVec = normalize(-viewVertex.xyz);
    
//    // From Shader.vsh
//    vec3 eyeNormal = normalize(NormalMatrix * Normal);
//    vec3 lightPosition = vec3(0.5, 1.0, 1.0);
//    //vec4 diffuseColor = vec4(0.4, 0.4, 1.0, 1.0);
//    float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));

    if ( ShadingType == 0 )
    {
        vColor = vec4(Material.Ambient, 0.0);
        for (int i = 0; i < 4; ++i)
        {
            if ( i >= NumLight )
                break;
            
            // Calculate diffuse term
            vec3 lightVec = normalize(Light[i].Position - vWorldVertex.xyz);
            //vec3 lightVec = normalize(Light[i].Position);
            //float l = dot(vWorldNormal, lightVec);
            float l = max(0.0, dot(vWorldNormal, lightVec));
            //float l = max(0.0, dot(eyeNormal, normalize(Light[i].Position)));;
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
                vec3 r = -normalize(reflect(lightVec, vWorldNormal));
                float s = pow(max(dot(r, vViewVec), 0.0), Material.Shininess);
                
                // Calculate attenuation factor
                float d = distance(vWorldVertex.xyz, Light[i].Position);
                float a = 1.0 / (Light[i].Attenuation.x + (Light[i].Attenuation.y * d) + (Light[i].Attenuation.z * d * d));
                
                // Add to color
                vColor.xyz += ((Material.Diffuse.xyz * l) + (Material.Specular * s)) * Light[i].Color * a * spotlight;
            }
        }
        
        vColor.w = Material.Diffuse.w;
    }
}