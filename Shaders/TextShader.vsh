// Attributes.
attribute vec3 Vertex;
attribute vec4 Color;
attribute vec2 TextureOffset;

// Varying variables.
varying vec4 vColor;
varying vec2 vTextureOffset;

// Vertex shader entry.
void main ()
{
    vColor = Color;
    vTextureOffset = TextureOffset;
    gl_Position = vec4(Vertex, 1.0);
}