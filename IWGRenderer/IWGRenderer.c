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
#include "IWColorTransition.h"
#include "IWFileTools.h"
#include "IWUIElement.h"
#include "IWCube.h"

#include "IWGameData.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void IWGRendererSetupGL(const char* vertexShaderFilename, const char* fragmentShaderFilename,
                        float viewWidth, float viewHeight)
{
    gdLightSourceData = IWGLightingMakeBasicLight();
    gdMaterialSourceData = IWGLightingMakeBasicMaterial();
        
    gdN_VERT = 0;
    
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
    //d = .2;
    
    //nx = ny = nz = 1;
    //d = 1.5;
    
    int n = nx * ny * nz;
    size_t mypos_size = n * 6 * 6 * 6 * sizeof(GLfloat);
    printf("Allocating %d position with total size %d\n", n * 72, (int)mypos_size);
    GLfloat *mypos = malloc(mypos_size);
    
    gdCubeToBufferMap = malloc(n * sizeof(unsigned int));
    gdBufferToCubeMap = malloc(n * sizeof(unsigned int));
    gdBufferToCubeMapNEntries = n;
    
    gdCubeData = IWCubeMakeCubeOfCube(nx, ny, nz, 1., .1);
    gdNCubes = nx * ny * nz;
    
    GLfloat *memPtr = mypos;
    for (int nc=0; nc < n; nc++) {
        gdCubeData[nc].triangleBufferData.bufferStart = mypos;
        gdCubeData[nc].triangleBufferData.start = memPtr;
        gdCubeData[nc].halfLengthX = d / 2.;
        gdCubeData[nc].collisionRadius = d * 1.1;
        memPtr += IWCubeToTriangles(&gdCubeData[nc]);
        // Setup primitive data buffer chain
        if (nc > 1) {
            gdCubeData[nc].triangleBufferData.previous = &gdCubeData[nc - 1].triangleBufferData;
            gdCubeData[nc - 1].triangleBufferData.next = &gdCubeData[nc].triangleBufferData;
        }
        gdCubeToBufferMap[nc] = nc;
        gdBufferToCubeMap[nc] = nc;
    }
    gdN_VERT = (memPtr - mypos) / 6;
    printf("nVertMax = %d\n", gdN_VERT);
    
    // Basic lighting program
    shaderProgramData = IWGShaderProgramMake(IWFileToolsReadFileToString(vertexShaderFilename),
                                             IWFileToolsReadFileToString(fragmentShaderFilename));
    GLuint programID = shaderProgramData.programID;
    
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
    IWGLightingSetUniforms(gdLightSourceData, gdMaterialSourceData);
    
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
    GLuint colorSlot = glGetAttribLocation(programID, "Color");
    
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normalSlot);
    glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
    
    //
    // Head up display data
    //
    glGenVertexArraysOES(1, &gdUITriangleVertexArray);
    glBindVertexArrayOES(gdUITriangleVertexArray);
    
    float aspect = fabsf(viewWidth / viewHeight);
    printf("aspect = %f\n", aspect);
    //IWVector4 squareButtonColor = {1.0, 1.0, 1.0, 0.3};
    // Saturated yellow
    //IWVector4 squareButtonColor = {255.0 / 255.0, 223. / 255., 94. / 255, 0.2};
    // White-ish yellow
    //IWVector4 squareButtonColor = {255.0 / 255.0, 236. / 255., 147. / 255, 0.3};
    // Light gray
    IWVector4 rectangleButtonColorTouched = {255.0 / 255.0, 236.0 / 255.0, 147.0 / 255.0, 0.6};
    IWVector4 rectangleButtonColorTouched2 = {0.8, 0.8, 0.8, 0.6};
    IWVector4 rectangleButtonColorTouched3 = {218.0 / 255.0, 255.0 / 255.0, 233.0 / 255.0, 0.6};
    IWVector4 rectangleButtonColorUntouched = {0.6, 0.6, 0.6, 0.25};
    IWVector4 rectangleButtonLineColor = {0.8, 0.8, 0.8, 0.5};
    gdRectangleButton = IWUIRectangleButtonMake(0.63, -0.001,
                                                IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                0.18, 0.18,
                                                rectangleButtonColorTouched, rectangleButtonColorUntouched,
                                                rectangleButtonLineColor,
                                                (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                0.035, aspect);
    gdRectangleButton2 = IWUIRectangleButtonMake(0.82, -0.001,
                                                 IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                 0.18, 0.18,
                                                 rectangleButtonColorTouched2, rectangleButtonColorUntouched,
                                                 rectangleButtonLineColor,
                                                 (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                 0.035, aspect);
    gdRectangleButton3 = IWUIRectangleButtonMake(0.44, -0.001,
                                                 IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                 0.18, 0.18,
                                                 rectangleButtonColorTouched3, rectangleButtonColorUntouched,
                                                 rectangleButtonLineColor,
                                                 (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                 0.035, aspect);

//    IWColorTransition colorTransition = {
//        {0.6, 0.6, 0.6, 0.4},
//        {0.8, 0.8, 0.8, 0.8},//{255.0 / 255.0, 236. / 255., 147. / 255, 0.5},
//        {0.6, 0.6, 0.6, 0.4},
//        0.5, 0.0, false, true
//    };
//    gdRectangleButton.colorTransition = colorTransition;
    gdUINTriangleVertices = (IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton)
                             + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton2)
                             + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton3)) / 7;
    
    size_t mypos_size2 = gdUINTriangleVertices * 7 * sizeof(GLfloat);
    GLfloat *mypos2 = malloc(mypos_size2);
    
    size_t offset = IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton, mypos2);
    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton2, mypos2 + offset);
    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton3, mypos2 + offset);

    //gdRectangleButton.color = IWVector4Make(255.0 / 255.0, 236. / 255., 147. / 255, 0.3);
    //IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);
    
    glGenBuffers(1, &gdUITriangleVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mypos_size2, mypos2, GL_DYNAMIC_DRAW);    
    
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
    
    glGenVertexArraysOES(1, &gdUILineVertexArray);
    glBindVertexArrayOES(gdUILineVertexArray);
    
    gdUINLineVertices = (IWUIRectangleButtonLineBufferSize(&gdRectangleButton)
                         + IWUIRectangleButtonLineBufferSize(&gdRectangleButton2)
                         + IWUIRectangleButtonLineBufferSize(&gdRectangleButton3)
                         + 2 * 42 * 7) / 7;
    
    mypos_size2 = gdUINLineVertices * 7 * sizeof(GLfloat);
    mypos2 = malloc(mypos_size2);
    
    offset = IWUIRectangleButtonToLineBuffer(&gdRectangleButton, mypos2);
    offset += IWUIRectangleButtonToLineBuffer(&gdRectangleButton2, mypos2 + offset);
    offset += IWUIRectangleButtonToLineBuffer(&gdRectangleButton3, mypos2 + offset);
    
    IWUIElement uiCentralCircle = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.03,//0.05,//0.3,//0.03,
                                                        IWVector4Make(1.0, 1.0, 1.0, 0.3), aspect, 31, mypos2 + offset);
    offset += uiCentralCircle.lineBufferSize;
    
    IWUIElement uiCentralCircle2 = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.001,//0.01,//0.1,//0.005,
                                                        IWVector4Make(1.0, 1.0, 1.0, 0.4), aspect, 11, mypos2 + offset);
    offset += uiCentralCircle2.lineBufferSize;
    
    glGenBuffers(1, &gdUILineVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gdUILineVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mypos_size2, mypos2, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));

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
    gdLightSourceData.Position = gdPlayerData.position;
    //_lightSource.Direction = IWVector3MultiplyScalar(playerData.direction, -1.0);
    gdLightSourceData.Direction = gdPlayerData.direction;
    IWGLightingSetUniforms(gdLightSourceData, gdMaterialSourceData);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT / 2);
    glDrawArrays(GL_TRIANGLES, 0, gdN_VERT);
    //glDrawArrays(GL_LINE_STRIP, 0, N_VERT / 2);
    
    glBindVertexArrayOES(0);
    
    // Draw array 2
    
    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArrayOES(gdUITriangleVertexArray);
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
                0);
    
    //glUseProgram(_program);
    
    //glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
    //glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 1.0, 0.1, 0.1, 1.0);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT2 / 2);
    glDrawArrays(GL_TRIANGLES, 0, gdUINTriangleVertices);

    glBindVertexArrayOES(gdUILineVertexArray);

    glLineWidth(0.5);
    glDrawArrays(GL_LINES, 0, gdUINLineVertices);
    
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
    glDeleteShader(shaderProgramData.vertexShaderID);
    glDeleteShader(shaderProgramData.fragmentShaderID);
    glDeleteProgram(shaderProgramData.programID);
    shaderProgramData.programID = 0;
}