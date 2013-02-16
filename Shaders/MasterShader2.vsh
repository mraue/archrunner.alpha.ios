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
//     0 : 2D flat shader
//     1 : Standard geometry shader with dynamic lighting
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
        
        vWorldVertex = ModelMatrix * vec4(Vertex, 1.0);
        
        vec4 viewVertex = ViewMatrix * vWorldVertex;
        
        gl_Position = ProjectionMatrix * ViewMatrix * vWorldVertex;// + 0.001;
        
        //gl_Position.z += 0.001;// prevent z fighting with HUD ??
        
        //vWorldNormal = normalize(mat3(NormalMatrix) * Normal);
        vWorldNormal = normalize(NormalMatrix * Normal);
        vViewVec.xyz = normalize(-viewVertex.xyz);
        
        //if ( LightingType == 0 )
        //{
            // World normal for lighting
            
            //vec3 eyeNormal = normalize(NormalMatrix * Normal);
            
            float sunCosAngle = max(0.0, dot(vWorldNormal, normalize(Sun.Direction)));
            float moonCosAngle = max(0.0, dot(vWorldNormal, normalize(Moon.Direction)));
            //vColor = Color * clamp(moonCosAngle * Moon.Color, 0.0, 1.0);
            vColor = Color * clamp(Sun.Color * (Sun.Color.w * sunCosAngle)
                                   + Moon.Color * (Moon.Color.w * moonCosAngle), 0.0, 1.0);
            
            // Calculate diffuse term
            vec3 lightVec = normalize(PlayerLight.Position - vWorldVertex.xyz);
            float l = max(0.0, dot(vWorldNormal, lightVec));
            
            //vColor = l * PlayerLight.Color;
            
            // REFACTOR: dynamic branching is bad ??? not sure ;)
            if ( l > 0.0 )
            {
                // Calculate spotlight effect
                float spotlight = 1.0;
                if ( PlayerLight.HasSpotlight != 0 )
                {
                    spotlight = max(-dot(lightVec, PlayerLight.Direction), 0.0);
                    float spotlightFade = clamp((PlayerLight.OuterCutoff - spotlight) / (PlayerLight.OuterCutoff - PlayerLight.InnerCutoff), 0.0, 1.0);
                    spotlight = pow(spotlight * spotlightFade, PlayerLight.Exponent);
                } else {
                    spotlight = max(-dot(lightVec, PlayerLight.Direction), 0.0);
                    spotlight = pow(spotlight, PlayerLight.VignettingExponent);
                }
                
                // Calculate attenuation factor
                float d = distance(vWorldVertex.xyz, PlayerLight.Position);
                float a = 1.0 / (PlayerLight.Attenuation.x + (PlayerLight.Attenuation.y * d) + (PlayerLight.Attenuation.z * d * d));
                
                // Add to color
                vColor.xyz += Color.xyz * l * PlayerLight.Color.xyz * a * spotlight * PlayerLight.Color.w;
                vColor.w = 1.0;
            }
            
        //} else {
        //    vColor = Color;
        //}
        
    }
    else if ( ShaderType == 2 )
    {
        vec3 eyeNormal = normalize(NormalMatrix * Normal);
        
        float nDotVP = max(0.0, dot(eyeNormal, normalize(Sun.Direction)));
        
        vColor = max(Color * nDotVP, Color * Sun.Color);
        
        gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Vertex, 1.0);
    }
    else if ( ShaderType == 3 )
    {
        vWorldVertex = ModelMatrix * vec4(Vertex, 1.0);
        
        vec4 viewVertex = ViewMatrix * vWorldVertex;
        
        gl_Position = ProjectionMatrix * viewVertex;
        
        //gl_Position.z += 0.001;// prevent z fighting with HUD ??
        
        vWorldNormal = normalize(NormalMatrix * Normal);
        vViewVec = normalize(-viewVertex.xyz);
    
        //vec3 eyeNormal = normalize(NormalMatrix * Normal);
        
        float sunCosAngle = max(0.0, dot(vWorldNormal, normalize(Sun.Direction)));
        float moonCosAngle = max(0.0, dot(vWorldNormal, normalize(Moon.Direction)));
        //vColor = Color * clamp(moonCosAngle * Moon.Color, 0.0, 1.0);
        vColor = Color * clamp(Sun.Color * (Sun.Color.w * sunCosAngle)
                               + Moon.Color * (Moon.Color.w * moonCosAngle), 0.0, 1.0);
        
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