
// Uniform variables.
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

// Attributes.
attribute vec3 Vertex;
attribute vec3 Normal;
attribute vec4 Color;

// Varying variables.
varying vec4 vColor;

// Vertex shader entry.
void main ()
{
    vColor = Color;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(Vertex, 1.0);
}