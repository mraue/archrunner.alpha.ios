//
//  ge_shaderprogram.c
//  gameone
//
//  Created by Martin Raue on 1/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "ge_shaderprogram.h"

#include <stdlib.h>
#include <stdio.h>

GLuint ge_build_program(const char* vertexShaderSource,
                        const char* fragmentShaderSource)
{
    GLuint vertexShader = ge_build_shader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = ge_build_shader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vertexShader);
    glAttachShader(prog, fragmentShader);
    glLinkProgram(prog);
    
    GLint linkSuccess;
    glGetProgramiv(prog, GL_LINK_STATUS, &linkSuccess);
    if (linkSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetProgramInfoLog(prog, sizeof(messages), NULL, &messages[0]);
        printf("Program link log:\n%s\n", messages);
        // Cleanup
        if (vertexShader) {
            glDeleteShader(vertexShader);
            vertexShader = 0;
        }
        if (fragmentShader) {
            glDeleteShader(fragmentShader);
            fragmentShader = 0;
        }
        if (prog) {
            glDeleteProgram(prog);
            prog = 0;
        }
        return 0;
    }

    return prog;
}

GLuint ge_build_shader(const char* source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint compileSuccess;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);
    if (compileSuccess == GL_FALSE) {
        GLchar messages[256];
        glGetShaderInfoLog(shader, sizeof(messages), NULL, &messages[0]);
        printf("Shader compile log:\n%s\n", messages);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}