#ifdef GL_ES
    precision highp float;
    precision highp int;
#endif
//default precision highp;

// Basic light source
struct BasicLightSource
{
    vec3 Direction;
    vec4 Color;
};

// Point light source
struct PointLightSource
{
    vec3 Position;
    vec3 Attenuation;
    vec3 Direction;
    vec4 Color;
    int HasSpotlight;
    float OuterCutoff;
    float InnerCutoff;
    float Exponent;
    float VignettingExponent;
};

// Uniform variables.
//uniform mat4 ProjectionMatrix;
//uniform mat4 ViewMatrix;
//uniform mat4 ModelMatrix;
//uniform mat3 NormalMatrix;

uniform BasicLightSource Sun;
uniform BasicLightSource Moon;
uniform PointLightSource PlayerLight;

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
varying mediump vec4 vWorldVertex;
varying mediump vec3 vWorldNormal;
varying mediump vec3 vViewVec;
varying lowp vec4 vColor;
varying vec2 vTextureOffset;

uniform lowp sampler2D Sampler;

// Fragment shader entry.
void main ()
{
    if ( ShaderType == 0)
    {
        gl_FragColor = vColor;        
    }
    else if ( ShaderType == 1 )
    {
//        if ( LightingType == 1 )
//        {
//            // vWorldNormal is interpolated when passed into the fragment shader.
//            // We need to renormalize the vector so that it stays at unit length.
//            vec3 normal = normalize(vWorldNormal);
//                
//            float sunCosAngle = max(0.0, dot(normal, normalize(Sun.Direction)));
//            float moonCosAngle = max(0.0, dot(normal, normalize(Moon.Direction)));
//            //vColor = Color * clamp(moonCosAngle * Moon.Color, 0.0, 1.0);
//            gl_FragColor = vColor * clamp(sunCosAngle * Sun.Color * Sun.Color.w
//                                   + moonCosAngle * Moon.Color * Moon.Color.w, 0.0, 1.0);
//            
//            // Calculate diffuse term
//            vec3 lightVec = normalize(PlayerLight.Position - vWorldVertex.xyz);
//            float l = max(0.0, dot(normal, lightVec));
//            //vColor = l * PlayerLight.Color;
//            
//            // REFACTOR: dynamic branching is bad ??? not sure ;)
//            if ( l > 0.0 )
//            {
//                // Calculate spotlight effect
//                float spotlight = 1.0;
//                if ( PlayerLight.HasSpotlight != 0 )
//                {
//                    spotlight = max(-dot(lightVec, PlayerLight.Direction), 0.0);
//                    float spotlightFade = clamp((PlayerLight.OuterCutoff - spotlight) / (PlayerLight.OuterCutoff - PlayerLight.InnerCutoff), 0.0, 1.0);
//                    spotlight = pow(spotlight * spotlightFade, PlayerLight.Exponent);
//                } else {
//                    spotlight = max(-dot(lightVec, PlayerLight.Direction), 0.0);
//                    spotlight = pow(spotlight, PlayerLight.VignettingExponent);
//                }
//                
//                // Calculate attenuation factor
//                float d = distance(vWorldVertex.xyz, PlayerLight.Position);
//                float a = 1.0 / (PlayerLight.Attenuation.x + (PlayerLight.Attenuation.y * d) + (PlayerLight.Attenuation.z * d * d));
//                
//                // Add to color
//                gl_FragColor.xyz += vColor.xyz * l * PlayerLight.Color.xyz * a * spotlight * PlayerLight.Color.w;
//                gl_FragColor.w = 1.0;
//            }
//        }
//        else
//        {
            // Set color
            gl_FragColor = vColor;
        //}
    }
    else if ( ShaderType == 5 )
    {
        gl_FragColor = texture2D(Sampler, vTextureOffset) * vColor;
    }
    else
    {
        gl_FragColor = vColor;
    }
}