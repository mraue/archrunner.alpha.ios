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

// Uniform variables.
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform vec2 GrayScale;

uniform BasicLightSource Sun;
uniform BasicLightSource Moon;
uniform PointLightSource PlayerLight;

// Varying variables.
varying vec4 vColor;

// Vertex shader entry.
void main ()
{
    //
    // Standard geometry shader with dynamic lighting
    //
    
    vec4 worldVertex = ModelMatrix * vec4(Vertex, 1.0);
    
    vec4 viewVertex = ViewMatrix * worldVertex;
    
    gl_Position = ProjectionMatrix * ViewMatrix * worldVertex;// + 0.001;
    
    //gl_Position.z += 0.001;// prevent z fighting with HUD ??
    
    //vWorldNormal = normalize(mat3(NormalMatrix) * Normal);
    vec3 worldNormal = normalize(NormalMatrix * Normal);
    //vViewVec.xyz = normalize(-viewVertex.xyz);
    
    // World normal for lighting
    
    //vec3 eyeNormal = normalize(NormalMatrix * Normal);
    
    float sunCosAngle = max(0.0, dot(worldNormal, normalize(Sun.Direction)));
    float moonCosAngle = max(0.0, dot(worldNormal, normalize(Moon.Direction)));
    //vColor = Color * clamp(moonCosAngle * Moon.Color, 0.0, 1.0);
    vColor = Color * clamp(Sun.Color * (Sun.Color.w * sunCosAngle)
                           + Moon.Color * (Moon.Color.w * moonCosAngle), 0.0, 1.0);
    
    // Calculate diffuse term
    vec3 lightVec = normalize(PlayerLight.Position - worldVertex.xyz);
    float l = max(0.0, dot(worldNormal, lightVec));
    
    //vColor = l * PlayerLight.Color;
    
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
    float d = distance(worldVertex.xyz, PlayerLight.Position);
    float a = 1.0 / (PlayerLight.Attenuation.x + (PlayerLight.Attenuation.y * d) + (PlayerLight.Attenuation.z * d * d));
    
    // Add to color
    vColor.xyz += Color.xyz * l * PlayerLight.Color.xyz * a * spotlight * PlayerLight.Color.w;
    vColor.w = 1.0;
    
    // to greyscale
    float averageBrightness = (vColor.r + vColor.g + vColor.b + vColor.a) / 4.0;
    // to lightgrey
    averageBrightness = (1.0 - GrayScale.y) + GrayScale.y * averageBrightness;
    vColor = vColor * (1.0 - GrayScale.x) + vec4(averageBrightness, averageBrightness, averageBrightness, 1.0) * GrayScale.x;
}