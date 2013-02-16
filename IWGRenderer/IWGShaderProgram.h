//
//  IWGShaderProgram.h
//  gameone
//
//  Created by Martin Raue on 1/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameone_IWGShaderProgram_h
#define gameone_IWGShaderProgram_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

typedef struct _IWGShaderProgramData {
    const char* vertexShaderFilename;
    const char* fragmentShaderFilename;
    const char* vertexShaderDataString;
    const char* fragmentShaderDataString;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;
    GLuint programID;
} IWGShaderProgramData;

IWGShaderProgramData IWGShaderProgramMake(const char* vertexShaderDataString,
                                          const char* fragmentShaderDataString);

void IWGShaderProgramInitFromFiles(IWGShaderProgramData *shaderProgram);
void IWGShaderProgramSetupPrograms(IWGShaderProgramData *shaderProgram);

GLuint IWGShaderProgramBuildShader(const char* source, GLenum shaderType);
GLuint IWGShaderProgramBuildProgram(const char* vertexShaderSource,
                                    const char* fragmentShaderSource);

#endif
