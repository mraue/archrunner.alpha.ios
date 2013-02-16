#ifdef GL_ES
    precision highp float;
    precision highp int;
#endif

// Varying variables.
varying lowp vec4 vColor;
varying vec2 vTextureOffset;

uniform lowp sampler2D Sampler;

// Fragment shader entry.
void main ()
{
    gl_FragColor = texture2D(Sampler, vTextureOffset) * vColor;
}