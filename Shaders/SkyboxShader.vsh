
// Uniform variables.
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec2 GrayScale;

// Attributes.
attribute vec3 Vertex;
attribute vec4 Color;

// Varying variables.
varying vec4 vColor;

// Vertex shader entry.
void main ()
{
    vColor = Color;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Vertex, 1.0);
    // to greyscale
    float averageBrightness = (vColor.r + vColor.g + vColor.b + vColor.a) / 4.0;
    // to lightgrey
    averageBrightness = (1.0 - GrayScale.y) + GrayScale.y * averageBrightness;
    vColor = vColor * (1.0 - GrayScale.x) + vec4(averageBrightness, averageBrightness, averageBrightness, 1.0) * GrayScale.x;
}