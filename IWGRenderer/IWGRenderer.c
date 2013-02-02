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

#include "IWUserInterface.h"

#include "IWColorTransition.h"
#include "IWFileTools.h"
#include "IWCube.h"
#include "IWFuel.h"

#include "IWGLighting.h"

#include "IWGBufferSubData.h"
//#include "IWGDoubleBuffer.h"

#include "IWGameData.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void IWGRendererSetupGL(const char* vertexShaderFilename, const char* fragmentShaderFilename,
                        float viewWidth, float viewHeight)
{
    gdLightSourceData = IWGLightingMakeBasicLight();
    gdMaterialSourceData = IWGLightingMakeBasicMaterial();
    
    IWColorTransition clearColorTransition = {
        {0.1, 0.1, 0.4, 1.0},
        {0.0, 0.0, 0.0, 1.0},
        {1.0, 1.0, 1.0, 1.0},
        1.0, 0.0, true, false
    };
    gdClearColorTransition = clearColorTransition;
    
    //gdB1TriangleNVertices = 0;
    
    //gePos mypos[55296];
    int nx, ny, nz;
    float d;
    
    //nx = ny = nz = 20;
    //d = 0.05;
    
    //nx = ny = nz = 27;
    //d = 0.05;
    
    nx = ny = nz = 5;
    //nx = ny = 7;
    //nz = 25;
    //d = .05;
    
    //nx = ny = nz = 4;
    //d = .2;
    
    //nx = ny = nz = 1;
    //d = 1.5;
    
    int n = nx * ny * nz;
    size_t mypos_size = n * 6 * 6 * 10 * sizeof(GLfloat);
    printf("Allocating %d position with total size %d\n", n * 6 * 6 * 10, (int)mypos_size);
    GLfloat *mypos = malloc(mypos_size);
    
    gdBufferToCubeMap = malloc(n * sizeof(unsigned int));
    gdBufferToCubeMapNEntries = n;

    gdStandardCubeIndexList = IWIndexListMake(n);
    
    IWVector4 cubeBaseColor = {0.4, 0.4, 1.0, 1.0};

    gdCubeData = IWCubeMakeCubes(nx, ny, nz, .05, .12, IWVector3Make(0.0, 0.0, 0.0), cubeBaseColor, 1, 0.05);
    gdNCubes = nx * ny * nz;
    
    IWVector3 *points = IWCubeMakeCubeCurve(gdNCubes, IWVector3Make(0.0, 0.0, 0.0), IWGEOMETRY_AXIS_Z);
    gdSecondaryPosition = points;
    gdSecondaryPositionCounter = 0;
    
    GLfloat *memPtr = mypos;
    for (int nc=0; nc < n; nc++) {
        // DBUG
        //gdCubeData[nc].secondaryPosition = points[nc];
        //gdCubeData[nc].type = IWCUBE_TYPE_OVERDRIVE;
        //gdCubeData[nc].color = IWUI_COLOR_GOLD(1.0);
        // DBUG
        gdCubeData[nc].triangleBufferData.bufferStartCPU = mypos;
        gdCubeData[nc].triangleBufferData.startCPU = memPtr;
        gdCubeData[nc].triangleBufferData.bufferIDGPU = nc;
        gdCubeData[nc].triangleBufferData.positionOffset = 0;
        gdCubeData[nc].triangleBufferData.colorOffset = 3;
        gdCubeData[nc].triangleBufferData.normalOffset = 7;
        gdCubeData[nc].triangleBufferData.stride = 10;
        memPtr += IWCubeToTriangles(&gdCubeData[nc]);
        // Setup primitive data buffer chain
        gdBufferToCubeMap[nc] = nc;
        gdStandardCubeIndexList.map[nc] = nc;
        gdStandardCubeIndexList.reverseMap[nc] = nc;
    }
    unsigned int nVertices = (memPtr - mypos) / gdCubeData[0].triangleBufferData.stride;
    
    //gdB1TriangleNVertices = (memPtr - mypos) / gdCubeData[0].triangleBufferData.stride;
    //gdB2TriangleNVertices = gdB1TriangleNVertices;
    printf("nVertices = %u\n", nVertices);
    
    // Basic lighting program
    shaderProgramData = IWGShaderProgramMake(IWFileToolsReadFileToString(vertexShaderFilename),
                                             IWFileToolsReadFileToString(fragmentShaderFilename));
    GLuint programID = shaderProgramData.programID;
    
    if (programID == 0) {
        printf("ERROR: Could not create GL program");
        return;
    }
    
    glUseProgram(programID);
    
    glEnable(GL_DEPTH_TEST);
    
    // Get attribute locations
    GLuint positionSlot = glGetAttribLocation(programID, "Vertex");
    GLuint normalSlot = glGetAttribLocation(programID, "Normal");
    GLuint colorSlot = glGetAttribLocation(programID, "Color");
    
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
    
    gdTriangleDoubleBuffer = IWGDoubleBufferGen();
    for (unsigned int  k =0; k < IWGDOUBLEBUFFER_MAX; k++) {
        gdTriangleDoubleBuffer.nVertices[k] = nVertices;
    }
    //gdTriangleDoubleBuffer.nVertices[0] = gdTriangleDoubleBuffer.nVertices[1] = gdB1TriangleNVertices;
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGDOUBLEBUFFER_MAX; i++) {
        IWGDoubleBufferBind(&gdTriangleDoubleBuffer, i);

        glBufferData(GL_ARRAY_BUFFER, mypos_size, mypos, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(normalSlot);
        glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
        //glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *ptr)
    }
    
    glBindVertexArrayOES(0);
    
    //
    // Head up display data
    //
    //glGenVertexArraysOES(1, &gdUITriangleVertexArray);
    //glBindVertexArrayOES(gdUITriangleVertexArray);
    
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
    gdRectangleButton = IWUIRectangleButtonMake(0.63, -0.001,
                                                IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                0.18, 0.19,
                                                rectangleButtonColorTouched, IWUI_COLOR_LIGHT_BLUE(0.25),
                                                IWUI_COLOR_LIGHT_BLUE(0.5),
                                                (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                0.035, aspect);
    gdRectangleButton2 = IWUIRectangleButtonMake(0.82, -0.001,
                                                 IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                 0.18, 0.19,
                                                 rectangleButtonColorTouched2, IWUI_COLOR_LIGHT_BLUE(0.25),
                                                 IWUI_COLOR_LIGHT_BLUE(0.5),
                                                 (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                 0.035, aspect);
    gdRectangleButton3 = IWUIRectangleButtonMake(0.44, -0.001,
                                                 IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                 0.18, 0.19,
                                                 rectangleButtonColorTouched3, IWUI_COLOR_LIGHT_BLUE(0.2),
                                                 IWUI_COLOR_LIGHT_BLUE(0.5),
                                                 (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                 0.035, aspect);

    gdFuel = IWFuelMakeDefaultStart();
    
    gdUINTriangleVertices = ((IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton)
                             + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton2)
                             + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton3)) / 7
                             + 18);
    
    size_t mypos_size2 = gdUINTriangleVertices * 7 * sizeof(GLfloat);
    GLfloat *mypos2 = malloc(mypos_size2);
    
    size_t offset = IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton, mypos2);
    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton2, mypos2 + offset);
    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton3, mypos2 + offset);
    gdFuel.stateBar.uiElementData.triangleBufferStart = mypos2 + offset;
    gdFuel.stateBar.uiElementData.triangleBufferSize = IWFuelToTriangleBuffer(&gdFuel, mypos2 + offset);
    offset += gdFuel.stateBar.uiElementData.triangleBufferSize;

    //gdRectangleButton.color = IWVector4Make(255.0 / 255.0, 236. / 255., 147. / 255, 0.3);
    //IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);
    
    gdUITriangleDoubleBuffer = IWGDoubleBufferGen();

    for (unsigned int i = 0; i < IWGDOUBLEBUFFER_MAX; i++) {
        IWGDoubleBufferBind(&gdUITriangleDoubleBuffer, i);
        glBufferData(GL_ARRAY_BUFFER, mypos_size2, mypos2, GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    }
    
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
    
    IWUIElementData uiCentralCircle = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.03,//0.05,//0.3,//0.03,
                                                        IWVector4Make(1.0, 1.0, 1.0, 0.3), aspect, 31, mypos2 + offset);
    offset += uiCentralCircle.lineBufferSize;
    
    IWUIElementData uiCentralCircle2 = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.001,//0.01,//0.1,//0.005,
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
//    
    return;
}

void IWGRendererRender(void)
{
    //glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    if (gdClearColorTransition.transitionHasFinished) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(gdClearColorTransition.currentColor.x,
                     gdClearColorTransition.currentColor.y,
                     gdClearColorTransition.currentColor.z,
                     gdClearColorTransition.currentColor.w);
    }
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glEnable(GL_CULL_FACE);
    
    // Draw cubes

    IWGDoubleBufferBindCurrentBuffer(&gdTriangleDoubleBuffer);
    
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
    
    // 
    //IWGLightingSetUniforms(gdLightSourceData, gdMaterialSourceData);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_POSITION],
                gdLightSourceData.Position.x, gdLightSourceData.Position.y, gdLightSourceData.Position.z);
    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_DIRECTION],
                gdLightSourceData.Direction.x, gdLightSourceData.Direction.y, gdLightSourceData.Direction.z);
    
    glDrawArrays(GL_TRIANGLES, 0, gdTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentBuffer]);
                 
    glBindVertexArrayOES(0);
    
    // Draw array 2
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //glBindVertexArrayOES(gdUITriangleVertexArray);
    IWGDoubleBufferBindCurrentBuffer(&gdUITriangleDoubleBuffer);
    
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
    glEnable(GL_DEPTH_TEST);
    
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