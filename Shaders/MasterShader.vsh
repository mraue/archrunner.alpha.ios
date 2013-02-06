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

// Attributes.
attribute vec3 Vertex;
attribute vec3 Normal;
attribute vec4 Color;
attribute vec2 TextureOffset;

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
//     0 : 2D flat shader
//     1 : Standard geometry shader with dynamic lighting (no specular term)
//     2 : Simple light shader
//
uniform int ShaderType;

// Varying variables.
varying vec4 vWorldVertex;
varying vec3 vWorldNormal;
varying vec3 vViewVec;
varying vec4 vColor;
varying vec2 vTextureOffset;

// Vertex shader entry.
void main ()
{
    if ( ShaderType == 0 )
    {
        gl_Position = vec4(Vertex, 1.0);
        vColor = Color;
    }
    else if ( ShaderType == 1 )
    {
        //
        // Standard geometry shader with dynamic lighting
        //
        
        //vWorldVertex = ModelMatrix * vec4(Vertex * ModelScale, 1.0);
        vWorldVertex = ModelMatrix * vec4(Vertex, 1.0);
        vec4 viewVertex = ViewMatrix * vWorldVertex;
        gl_Position = ProjectionMatrix * viewVertex;
        gl_Position.z += 0.001;// prevent z fighting with HUD ??
        // World normal for lighting
        vWorldNormal = normalize(mat3(NormalMatrix) * Normal);    
        vViewVec = normalize(-viewVertex.xyz);

        if ( LightingType == 0 )
        {
            vColor = vec4(Material.Ambient, 0.0);
            for (int i = 0; i < 4; ++i)
            {
                if ( i >= NumLight )
                    break;
                
                // Calculate diffuse term
                vec3 lightVec = normalize(Light[i].Position - vWorldVertex.xyz);
                float l = max(0.0, dot(vWorldNormal, lightVec));

                // REFACTOR: dynamic branching is bad ??? not sure ;)
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
                    //vec3 r = -normalize(reflect(lightVec, vWorldNormal));
                    //float s = pow(max(dot(r, vViewVec), 0.0), Material.Shininess);
                    
                    // Calculate attenuation factor
                    float d = distance(vWorldVertex.xyz, Light[i].Position);
                    float a = 1.0 / (Light[i].Attenuation.x + (Light[i].Attenuation.y * d) + (Light[i].Attenuation.z * d * d));
                    
                    // Add to color
                    //vColor.xyz += Material.Diffuse.xyz * l * Light[i].Color * a * spotlight;
                    vColor.xyz += Color.xyz * l * Light[i].Color * a * spotlight;
                    vColor.w = Color.w;
                }
            }
            //vColor.w = Material.Diffuse.w;
        }
        else
        {
            vColor = Color;
        }
        
    }
    else if ( ShaderType == 2 )
    {
        vec3 eyeNormal = normalize(NormalMatrix * Normal);
        
        float nDotVP = max(0.0, dot(eyeNormal, normalize(Light[0].Position)));
        
        vColor = max(Color * nDotVP, Color * vec4(Material.Ambient, 1.0));
        
        gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Vertex, 1.0);
    }
    else if ( ShaderType == 3 )
    {
        vec3 eyeNormal = normalize(NormalMatrix * Normal);
        
        float nDotVP = max(0.0, dot(eyeNormal, normalize(Light[0].Position)));
        
        vColor = max(Color * nDotVP, Color * vec4(Material.Ambient, 1.0));
        
        // to greyscale
        float colors = (vColor.r + vColor.g + vColor.b + vColor.a) / 4.0;
        // to lightgrey
        colors = 0.6 + 0.4 * colors;
        vColor = vec4(colors, colors, colors, 1.0);
        
        gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Vertex, 1.0);
    }
    else if ( ShaderType == 4 )
    {
        //vColor = Color.xyz;
        vColor = Color;
        gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Vertex, 1.0);
    }
    else if ( ShaderType == 5 )
    {
        //vColor = Color.xyz;
        vColor = Color;
        vTextureOffset = TextureOffset;
        gl_Position = vec4(Vertex, 1.0);
    }
}