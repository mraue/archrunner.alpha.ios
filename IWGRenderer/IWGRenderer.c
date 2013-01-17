//
//  IWGRenderer.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGRenderer.h"

#include <stdio.h>
#include <stdlib.h>

#include <GLKit/GLKMath.h>

#include "IWUIRectangleButton.h"
#include "IWFileTools.h"

#include "IWGameData.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void IWGRendererSetupGL(const char* vertexShaderFilename, const char* fragmentShaderFilename,
                        float viewWidth, float viewHeight)
{
    lightSourceData = IWGLightingMakeBasicLight();
    materialSourceData = IWGLightingMakeBasicMaterial();
    
    //gePos mypos[55296];
    int nx, ny, nz;
    float d;
    
    //nx = ny = nz = 20;
    //d = 0.05;
    
    //nx = ny = nz = 27;
    //d = 0.05;
    
    nx = ny = nz = 10;
    d = .05;
    
    //nx = ny = nz = 4;
    //d = .1;
    
    //nx = ny = nz = 1;
    //d = 1.5;
    
    int n = nx * ny * nz;
    size_t mypos_size = n * 72 * sizeof(gePos);
    printf("Allocating %d position with total size %d\n", n * 72, (int)mypos_size);
    gePos *mypos = malloc(mypos_size);
    
    geCube* mycubes = ge_create_uniform_cubes(nx, ny, nz, 1., .1);
    
    for (int nc=0; nc < n; nc++) {
        //ge_cube_to_trianglestrip(&mycubes[nc + 1], &mypos[N_VERT], d, 1, 1, &N_VERT);
        ge_cube_to_triangles(&mycubes[nc], &mypos[gdN_VERT], d, &gdN_VERT);
    }
    printf("nVertMax = %d\n", gdN_VERT);
    
    // Basic lighting program
//    programData = IWGShaderProgramMake(IWFileToolsReadFileToString(vertexShaderFilename),
//                                       IWFileToolsReadFileToString(fragmentShaderFilename));
    programData = IWGShaderProgramMake(vertexShaderFilename,
                                       fragmentShaderFilename);
    GLuint programID = programData.programID;
    
    if (programID == 0) {
        printf("ERROR: Could not create GL program");
        return;
    }
    
    // Get uniform locations.
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX]
        = glGetUniformLocation(programID, "ModelMatrix");
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX]
        = glGetUniformLocation(programID, "ViewMatrix");
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX]
        = glGetUniformLocation(programID, "ProjectionMatrix");
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_NORMAL_MATRIX]
        = glGetUniformLocation(programID, "NormalMatrix");

    IWGLightingInitializeUniformLocations(programID);
    IWGLightingSetUniforms(lightSourceData, materialSourceData);
    
    //glUseProgram(_program);
    glUseProgram(programID);
    
    glEnable(GL_DEPTH_TEST);
    
    // First cube
    glGenVertexArraysOES(1, &gdVertexArray);
    glBindVertexArrayOES(gdVertexArray);
    
    glGenBuffers(1, &gdVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gdVertexBuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertexData), gCubeVertexData, GL_STATIC_DRAW);
    
    //glBufferData(GL_ARRAY_BUFFER, mypos_size, mypos, GL_STATIC_DRAW);
    // DEBUG
    glBufferData(GL_ARRAY_BUFFER, mypos_size, mypos, GL_DYNAMIC_DRAW);
    // DEBUG END
    
    GLuint positionSlot = glGetAttribLocation(programID, "Vertex");
    GLuint normalSlot = glGetAttribLocation(programID, "Normal");
    GLuint colourSlot = glGetAttribLocation(programID, "Colour");
    
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normalSlot);
    glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
    
    //
    // Head up display data
    //
    glGenVertexArraysOES(1, &gdVertexArray2);
    glBindVertexArrayOES(gdVertexArray2);
    
    float aspect = fabsf(viewWidth / viewHeight);
    printf("aspect = %f\n", aspect);
    //IWVector4 squareButtonColour = {1.0, 1.0, 1.0, 0.3};
    // Saturated yellow
    //IWVector4 squareButtonColour = {255.0 / 255.0, 223. / 255., 94. / 255, 0.2};
    // White-ish yellow
    //IWVector4 squareButtonColour = {255.0 / 255.0, 236. / 255., 147. / 255, 0.3};
    // Light gray
    IWVector4 rectangleButtonColour = {0.6, 0.6, 0.6, 0.4};
    IWUIRectangleButton rectangleButton = IWUIRectangleButtonMake(0.4, 0.0,
                                                                  IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                                  0.18, 0.18, rectangleButtonColour,
                                                                  IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT,
                                                                  0.035, aspect);
    
    size_t mypos_size2 = 4 * 9 * 7 * sizeof(GLfloat);
    GLfloat *mypos2 = malloc(mypos_size2);
    
    IWUIRectangleButtonToTriangleBuffer(&rectangleButton, mypos2);
    
    glGenBuffers(1, &gdVertexBuffer2);
    glBindBuffer(GL_ARRAY_BUFFER, gdVertexBuffer2);
    glBufferData(GL_ARRAY_BUFFER, mypos_size2, mypos2, GL_DYNAMIC_DRAW);
    
    
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(colourSlot);
    glVertexAttribPointer(colourSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
    
//    // Slowed it down :(
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
//    glFrontFace(GL_CW);
//    
    return;
}

void IWGRendererRender(void)
{
    //glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw array 1
    glBindVertexArrayOES(gdVertexArray);
    
    //glUseProgram(_program);
    //glUseProgram(_program);
    
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX],
                       1, 0, gdModelMatrix.m);
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX],
                       1, 0, gdViewMatrix.m);
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX],
                       1, 0, gdProjectionMatrix.m);
    glUniformMatrix3fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_NORMAL_MATRIX],
                       1, 0, gdNormalMatrix.m);

    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 0.4, 0.4, 1.0, 1.0);
    lightSourceData.Position = gdPlayerData.position;
    //_lightSource.Direction = IWVector3MultiplyScalar(playerData.direction, -1.0);
    lightSourceData.Direction = gdPlayerData.direction;
    IWGLightingSetUniforms(lightSourceData, materialSourceData);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT / 2);
    glDrawArrays(GL_TRIANGLES, 0, gdN_VERT / 2);
    //glDrawArrays(GL_LINE_STRIP, 0, N_VERT / 2);
    
    glBindVertexArrayOES(0);
    
    // Draw array 2
    
    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArrayOES(gdVertexArray2);
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
                0);
    
    //glUseProgram(_program);
    
    //glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
    //glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 1.0, 0.1, 0.1, 1.0);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT2 / 2);
    glDrawArrays(GL_TRIANGLES, 0, 1*9 + 3*6);
    
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
                1);
    
    glBindVertexArrayOES(0);
    glDisable(GL_BLEND);
    return;
}

void IWGRendererTearDownGL(void)
{
    // Delete buffer
    //glDeleteBuffers(GLsizei n, const GLuint *buffers)
    // Delete shader and programs
    glDeleteShader(programData.vertexShaderID);
    glDeleteShader(programData.fragmentShaderID);
    glDeleteProgram(programData.programID);
    programData.programID = 0;
}