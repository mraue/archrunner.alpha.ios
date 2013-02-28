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

typedef enum {
    IWGSHADERPROGRAM_VARYING_SLOTS_VERTEX  = 1 <<  0,
    IWGSHADERPROGRAM_VARYING_SLOTS_COLOR  = 1 <<  1,
    IWGSHADERPROGRAM_VARYING_SLOTS_NORMAL = 1 <<  2,
    IWGSHADERPROGRAM_VARYING_SLOTS_TEXTURE_OFFSET = 1 <<  3,
    IWGSHADERPROGRAM_VARYING_SLOTS_ALL = (1 << 4) - 1,
    IWGSHADERPROGRAM_VARYING_SLOTS_SCENE =
    IWGSHADERPROGRAM_VARYING_SLOTS_VERTEX | IWGSHADERPROGRAM_VARYING_SLOTS_COLOR | IWGSHADERPROGRAM_VARYING_SLOTS_NORMAL,
    IWGSHADERPROGRAM_VARYING_SLOTS_UI =
    IWGSHADERPROGRAM_VARYING_SLOTS_VERTEX | IWGSHADERPROGRAM_VARYING_SLOTS_COLOR,
    IWGSHADERPROGRAM_VARYING_SLOTS_TEXT =
    IWGSHADERPROGRAM_VARYING_SLOTS_VERTEX | IWGSHADERPROGRAM_VARYING_SLOTS_COLOR | IWGSHADERPROGRAM_VARYING_SLOTS_TEXTURE_OFFSET
} IWGSHADERPROGRAM_VARYING_SLOTS;

typedef struct _IWGShaderProgramData {
    const char* vertexShaderFilename;
    const char* fragmentShaderFilename;
    const char* vertexShaderDataString;
    const char* fragmentShaderDataString;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;
    GLuint programID;
    GLint vertexSlot, colorSlot, normalSlot, textureOffsetSlot;
    IWGSHADERPROGRAM_VARYING_SLOTS varyingSlots;
} IWGShaderProgramData;


IWGShaderProgramData IWGShaderProgramMake(const char* vertexShaderDataString,
                                          const char* fragmentShaderDataString,
                                          IWGSHADERPROGRAM_VARYING_SLOTS varyingSlots);

IWGShaderProgramData IWGShaderProgramMakeFromFiles(const char* vertexShaderFilename,
                                                   const char* fragmentShaderFilename,
                                                   IWGSHADERPROGRAM_VARYING_SLOTS varyingSlots);

void IWGShaderProgramInitFromFiles(IWGShaderProgramData *shaderProgram);
void IWGShaderProgramSetupPrograms(IWGShaderProgramData *shaderProgram);

GLuint IWGShaderProgramBuildShader(const char* source, GLenum shaderType);
GLuint IWGShaderProgramBuildProgram(const char* vertexShaderSource,
                                    const char* fragmentShaderSource);

#endif
