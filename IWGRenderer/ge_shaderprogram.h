//
//  ge_shaderprogram.h
//  gameone
//
//  Created by Martin Raue on 1/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameone_ge_shaderprogram_h
#define gameone_ge_shaderprogram_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

GLuint ge_build_shader(const char* source, GLenum shaderType);
GLuint ge_build_program(const char* vertexShaderSource,
                        const char* fragmentShaderSource);

#endif
