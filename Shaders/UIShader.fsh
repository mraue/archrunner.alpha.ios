#ifdef GL_ES
precision highp float;
precision highp int;
#endif

// Varying variables
varying lowp vec4 vColor;

// Fragment shader entry.
void main ()
{
    gl_FragColor = vColor;
}