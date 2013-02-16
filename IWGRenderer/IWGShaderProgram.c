//
//  IWGShaderProgram.c
//  gameone
//
//  Created by Martin Raue on 1/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGShaderProgram.h"

#include <stdlib.h>
#include <stdio.h>

#include "IWFileTools.h"

IWGShaderProgramData IWGShaderProgramMake(const char* vertexShaderDataString,
                                          const char* fragmentShaderDataString)
{
    IWGShaderProgramData programData = {
        "", "", vertexShaderDataString, fragmentShaderDataString, 0, 0, 0
    };
    
    IWGShaderProgramSetupPrograms(&programData);
    
    return programData;
}

void IWGShaderProgramInitFromFiles(IWGShaderProgramData *shaderProgram)
{
    shaderProgram->vertexShaderDataString = IWFileToolsReadFileToString(shaderProgram->vertexShaderFilename);
    shaderProgram->fragmentShaderDataString = IWFileToolsReadFileToString(shaderProgram->fragmentShaderFilename);
    IWGShaderProgramSetupPrograms(shaderProgram);
}

void IWGShaderProgramSetupPrograms(IWGShaderProgramData *shaderProgram)
{
    GLuint vertexShader = IWGShaderProgramBuildShader(shaderProgram->vertexShaderDataString, GL_VERTEX_SHADER);
    GLuint fragmentShader = IWGShaderProgramBuildShader(shaderProgram->fragmentShaderDataString, GL_FRAGMENT_SHADER);
    
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
        return;
    }
    shaderProgram->programID = prog;
    shaderProgram->vertexShaderID = vertexShader;
    shaderProgram->fragmentShaderID = fragmentShader;
    
    return;
}

GLuint IWGShaderProgramBuildProgram(const char* vertexShaderSource,
                                    const char* fragmentShaderSource)
{
    GLuint vertexShader = IWGShaderProgramBuildShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = IWGShaderProgramBuildShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
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

GLuint IWGShaderProgramBuildShader(const char* source, GLenum shaderType)
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