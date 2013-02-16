// Attributes.
attribute vec3 Vertex;
attribute vec4 Color;

// Varying variables.
varying vec4 vColor;

// Vertex shader entry.
void main ()
{
    gl_Position = vec4(Vertex, 1.0);
    vColor = Color;
}