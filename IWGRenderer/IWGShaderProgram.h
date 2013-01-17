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

struct _IWGShaderProgramData {
    const char* vertexShaderDataString;
    const char* fragmentShaderDataString;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;
    GLuint programID;
};
typedef struct _IWGShaderProgramData IWGShaderProgramData;

IWGShaderProgramData IWGShaderProgramMake(const char* vertexShaderDataString,
                                          const char* fragmentShaderDataString);

GLuint IWGShaderProgramBuildShader(const char* source, GLenum shaderType);
GLuint IWGShaderProgramBuildProgram(const char* vertexShaderSource,
                                    const char* fragmentShaderSource);

#endif
