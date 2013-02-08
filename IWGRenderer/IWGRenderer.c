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
#include "IWCubeCounter.h"
#include "IWGCircle.h"

#include "IWGLighting.h"

#include "IWGBufferSubData.h"
//#include "IWGMultiBuffer.h"

#include "IWGFontMap.h"
#include "IWGFontMapEntry.h"
#include "IWGTextLine.h"
#include "IWGTextField.h"

#include "IWGameData.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void IWGRendererSetupGL(const char* vertexShaderFilename,
                        const char* fragmentShaderFilename,
                        const char* fontMapFilename,
                        float viewWidth,
                        float viewHeight)
{
    gdMasterShaderID = 2;
    gdSkyShaderID = 4;
    
    gdClearColor = IWVector4Make(0.6, 0.6, 0.6, 1.0);
    //gdClearColor = IWVector4Make(0.95, 0.95, 0.95, 1.0);
    
    gdLightSourceData = IWGLightingMakeBasicLight();
    gdMaterialSourceData = IWGLightingMakeBasicMaterial();
    
    IWColorTransition clearColorTransition = {
        {0.8, 0.8, 0.8, 1.0},
        gdClearColor,
        {0.8, 0.8, 0.8, 1.0},
        0.8, 0.0, true, false
    };
    gdClearColorTransition = clearColorTransition;
    
    IWColorTransition overdriveColorTransition = {
        IWUI_COLOR_WHITE(0.4),
        IWUI_COLOR_WHITE(1.0),
        IWUI_COLOR_WHITE(0.4),
        0.4, 0.0, true, true
    };
    gdOverdriveColorTransition = overdriveColorTransition;
    
    int nx, ny, nz;
    
    nx = ny = nz = 5;// 8
    
    int n = nx * ny * nz;
    size_t mypos_size = n * 6 * 6 * 10 * sizeof(GLfloat);
    printf("Allocating %d position with total size %d\n", n * 6 * 6 * 10, (int)mypos_size);
    GLfloat *mypos = malloc(mypos_size);

    gdStandardCubeIndexList = IWIndexListMake(n);
    gdPoolCubeIndexList = IWIndexListMake(n);
    gdPoolCubeIndexList.nEntries = 0;
    gdGPUBufferPositionIndexList = IWIndexListMake(n);
    
    IWVector4 cubeBaseColor = {0.4, 0.4, 1.0, 1.0};

    //gdCubeData = IWCubeMakeCubes(nx, ny, nz, .05, .12, IWVector3Make(0.0, 0.0, 0.0), cubeBaseColor, 1, 0.05);
    gdCubeData = IWCubeMakeCubes(nx, ny, nz, .04, .12, IWVector3Make(0.0, 0.0, 0.0), cubeBaseColor, 1, 0.05);
    gdNCubes = nx * ny * nz;
    
    gdSecondaryPosition = IWCubeMakeCubeCurve(gdNCubes, IWVector3Make(0.0, 0.0, 0.0), IWGEOMETRY_AXIS_Z);
    gdSecondaryPositionCounter = 0;
    
    GLfloat *memPtr = mypos;
    for (int nc=0; nc < n; nc++) {
        // DBUG
        //gdCubeData[nc].secondaryPosition = points[nc];
        //gdCubeData[nc].type = IWCUBE_TYPE_OVERDRIVE;
        //gdCubeData[nc].color = IWUI_COLOR_GOLD(1.0);
        // DBUG
        // Spawn cube
        gdCubeData[nc].type = IWCUBE_TYPE_SPAWNING;
        gdCubeData[nc].isInteractive = false;
        
        IWVector3 spawnPosition = IWVector3MultiplyScalar(IWVector3Normalize(gdCubeData[nc].centerPosition),
                                                          IW_RAND_UNIFORM(2.0, 4.0));
        float transitionTime = 2.5;
        
        gdCubeData[nc].positionTransition = IWVector3TransitionMake(spawnPosition,
                                                                    gdCubeData[nc].centerPosition,
                                                                    spawnPosition,
                                                                    transitionTime, 0.0, false, false);
        gdCubeData[nc].centerPosition = spawnPosition;
        //gdCubeData[nc].color = IWUI_COLOR_DARK_BLUE(1.0);
        gdCubeData[nc].color = IWVector4Make(0.5, 0.5, 0.5, 1.0);
        //
        gdCubeData[nc].triangleBufferData.bufferStartCPU = mypos;
        gdCubeData[nc].triangleBufferData.startCPU = memPtr;
        gdCubeData[nc].triangleBufferData.bufferIDGPU = nc;
        //
        //gdCubeData[nc].faces = IWCUBE_FACES_BOWL;
        //
        gdCubeData[nc].triangleBufferData.bufferOffsetGPU = memPtr - mypos;
        //
        memPtr += IWCubeToTriangles(&gdCubeData[nc]);
        // Setup primitive data buffer chain
        //gdStandardCubeIndexList.map[nc] = nc;
        //gdStandardCubeIndexList.reverseMap[nc] = nc;
        IWIndexListAppendObjectId(&gdStandardCubeIndexList, nc);
        IWIndexListAppendObjectId(&gdGPUBufferPositionIndexList, nc);
    }
    
    unsigned int nVertices = (memPtr - mypos) / gdCubeData[0].triangleBufferData.stride;
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
    GLuint textureOffsetSlot = glGetAttribLocation(programID, "TextureOffset");
    
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
    
    gdTriangleDoubleBuffer = IWGMultiBufferGen();
    for (unsigned int  k =0; k < IWGMULTIBUFFER_MAX; k++) {
        gdTriangleDoubleBuffer.nVertices[k] = nVertices;
    }
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        IWGMultiBufferBind(&gdTriangleDoubleBuffer, i);

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
    // Sky box
    //
    
    gdSkyCube= IWCubeMake(n, IWCUBE_TYPE_STANDARD,
                          IWVector3Make(gdPlayerData.position.x, -10.0, gdPlayerData.position.z),
                          IWVector4Make(0.5, 0.5, 0.5, 1.0),
                          IWVector3Make(60.0, 20.0, 60.0),
                          IWCUBE_FACES_BOWL,
                          IWCUBE_NORMALS_INWARD,
                          0.0, true, false, IWVector3TransitionMakeEmpty());

    gdSun = IWGCircleMake(IWVector3Make(0.0, -1.0, 30.5), IWVector3Make(0.0, 0.0, 1.0), IWUI_COLOR_GOLD(1.0), 5.0, 41);

    size_t skySize = (1 * 5 * 6 + gdSun.nTriangles * 3)* 10 * sizeof(GLfloat);
    gdSkyCube.triangleBufferData.startCPU = malloc(skySize);
    
    gdSkyCube.triangleBufferData.size = IWCubeToTriangles(&gdSkyCube);

    gdSun.triangleBufferData.startCPU = gdSkyCube.triangleBufferData.startCPU + gdSkyCube.triangleBufferData.size;
    gdSun.triangleBufferData.size = IWGCircleToTriangles(&gdSun);

    glGenVertexArraysOES(1, &gdSkyTriangleVertexArray);
    glBindVertexArrayOES(gdSkyTriangleVertexArray);
    
    glGenBuffers(1, &gdSkyTriangleVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gdSkyTriangleVertexBuffer);
    
    glBufferData(GL_ARRAY_BUFFER, skySize, gdSkyCube.triangleBufferData.startCPU, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(normalSlot);
    glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    
    glBindVertexArrayOES(0);
    
    //
    // Text
    //
    float aspect = fabsf(viewWidth / viewHeight);
    
    GLuint textureHandlerId;
    glGenTextures(1, &textureHandlerId);
    
    gdFontMap = IWGFontMapCreateFromFile(fontMapFilename);
    
//    IWGTextLineData textLine = IWGTextLineDataMake("67",
//                                                   IWVector2Make(0.6, 0.5),
//                                                   0.1,
//                                                   IWUI_COLOR_GOLD(1.0),
//                                                   1. / aspect,
//                                                   &gdFontMap);
//    IWGTextFieldData textField = IWGTextFieldMake(IWVector2Make(0.0, 0.0),
//                                                  IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
//                                                  3, 11,
//                                                  1. / aspect,
//                                                  "HELLO WORLD\nMY NAME IS\nGAME ONE",
//                                                  0.2, -0.05,
//                                                  IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
//                                                  IWUI_COLOR_GOLD(0.8),
//                                                  &gdFontMap,
//                                                  NULL);
    gdScoreTextField = IWGTextFieldMake(IWVector2Make(0.95, 1.0),
                                        IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                                        1, 10,
                                        1. / aspect,
                                        "",
                                        0.4, -0.04,
                                        IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                        IWVector4Make(1.0, 1.0, 1.0, 0.7),
                                        &gdFontMap,
                                        NULL);
    
    gdTextTriangleDoubleBuffer = IWGMultiBufferGen();
    for (unsigned int  k =0; k < IWGMULTIBUFFER_MAX; k++) {
        gdTextTriangleDoubleBuffer.nVertices[k] = gdScoreTextField.triangleBufferData.size / gdScoreTextField.triangleBufferData.stride;
    }
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {

        IWGMultiBufferBind(&gdTextTriangleDoubleBuffer, i);
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, gdFontMapTextureData);

        glBufferData(GL_ARRAY_BUFFER, gdScoreTextField.triangleBufferData.size * sizeof(GLfloat),
                     gdScoreTextField.triangleBufferData.startCPU,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textureOffsetSlot);
        glVertexAttribPointer(textureOffsetSlot, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    
    glBindVertexArrayOES(0);
    
    //
    // Head up display data
    //
    printf("aspect = %f\n", aspect);
    
    // Some buttons
    gdRectangleButton = IWUIRectangleButtonMake(0.0, -0.001,
                                                IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
                                                0.18, 0.19,
                                                IWUI_COLOR_WHITE(0.5),
                                                IWUI_COLOR_WHITE(0.25),
                                                IWUI_COLOR_WHITE(0.5),
                                                (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_RIGHT),
                                                0.035, 1. / aspect);
    gdRectangleButton2 = IWUIRectangleButtonMake(0.82, -0.001,
                                                 IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
                                                 0.18, 0.19,
                                                 IWUI_COLOR_PURPLE(0.3), IWUI_COLOR_WHITE(0.25),
                                                 IWUI_COLOR_WHITE(0.5),
                                                 (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                 0.035, 1. / aspect);
    gdRectangleButton3 = IWUIRectangleButtonMake(0.63, -0.001,
                                                 IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT,
                                                 0.18, 0.19,
                                                 IWUI_COLOR_GOLD(0.3), IWUI_COLOR_WHITE(0.25),
                                                 IWUI_COLOR_WHITE(0.5),
                                                 (IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT),
                                                 0.035, 1. / aspect);

    gdFuel = IWFuelMakeDefaultStart();
    //gdFuel.stateBar.direction = IWUI_DIRECTION_REVERSE;// need larger buffer! -> 4 x 6
    
    gdCubeCounter = IWCubeCounterMake(n);
    gdCubeCounter.spawned = n;
    IWCubeCounterUpdateStateBar(&gdCubeCounter);

    gdUINTriangleVertices = ((IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton)
                             + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton2)
                             + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton3)) / 7
                             + 6 * 3// 4 for reverse, 3 for normal
                             //+ 6 * 4// cube counter
                             );
    
    size_t mypos_size2 = gdUINTriangleVertices * 7 * sizeof(GLfloat);
    GLfloat *mypos2 = malloc(mypos_size2);
    
    gdRectangleButton.triangleBuffer.bufferOffsetGPU = 0;    
    size_t offset = IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton, mypos2);
    gdRectangleButton2.triangleBuffer.bufferOffsetGPU = offset;
    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton2, mypos2 + offset);
    gdRectangleButton3.triangleBuffer.bufferOffsetGPU = offset;
    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton3, mypos2 + offset);
    
    gdFuel.stateBar.triangleBufferData.bufferStartCPU = mypos2 + offset;
    gdFuel.stateBar.triangleBufferData.bufferOffsetGPU = offset;
    gdFuel.stateBar.triangleBufferData.size = IWFuelToTriangleBuffer(&gdFuel, mypos2 + offset);
    offset += gdFuel.stateBar.triangleBufferData.size;
    
//    gdCubeCounter.stateBar.triangleBufferData.bufferStartCPU = mypos2 + offset;
//    gdCubeCounter.stateBar.triangleBufferData.bufferOffsetGPU = offset;
//    gdCubeCounter.stateBar.triangleBufferData.size = IWUIStateBarToTriangles(&gdCubeCounter.stateBar);
//    offset += gdCubeCounter.stateBar.triangleBufferData.size;
    
    //gdRectangleButton.color = IWVector4Make(255.0 / 255.0, 236. / 255., 147. / 255, 0.3);
    //IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);
    
    gdUITriangleDoubleBuffer = IWGMultiBufferGen();

    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        IWGMultiBufferBind(&gdUITriangleDoubleBuffer, i);
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
                         + 2 * 52 * 7) / 7;
    
    mypos_size2 = gdUINLineVertices * 7 * sizeof(GLfloat);
    mypos2 = malloc(mypos_size2);
    
    gdRectangleButton.lineBuffer.bufferOffsetGPU = 0;
    offset = IWUIRectangleButtonToLineBuffer(&gdRectangleButton, mypos2);
    gdRectangleButton2.lineBuffer.bufferOffsetGPU = offset;
    offset += IWUIRectangleButtonToLineBuffer(&gdRectangleButton2, mypos2 + offset);
    gdRectangleButton3.lineBuffer.bufferOffsetGPU = offset;
    offset += IWUIRectangleButtonToLineBuffer(&gdRectangleButton3, mypos2 + offset);
    
    IWUIElementData uiCentralCircle = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.06,//0.05,//0.3,//0.03,
                                                        IWVector4Make(1.0, 1.0, 1.0, 0.3), aspect, 41, mypos2 + offset);
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
//    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
//
    return;
}

void IWGRendererRender(void)
{
    //glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    if (gdClearColorTransition.transitionHasFinished) {
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearColor(gdClearColor.x, gdClearColor.y, gdClearColor.z, gdClearColor.w);
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

    IWGMultiBufferBindCurrentDrawBuffer(&gdTriangleDoubleBuffer);
    
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX],
                       1, 0, gdModelMatrix.m);
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX],
                       1, 0, gdViewMatrix.m);
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX],
                       1, 0, gdProjectionMatrix.m);
    glUniformMatrix3fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_NORMAL_MATRIX],
                       1, 0, gdNormalMatrix.m);

//    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 0.4, 0.4, 1.0, 1.0);
//    gdLightSourceData.Position = gdPlayerData.position;
//    //_lightSource.Direction = IWVector3MultiplyScalar(playerData.direction, -1.0);
//    gdLightSourceData.Direction = gdPlayerData.direction;
//    
    //
    //IWGLightingSetUniforms(gdLightSourceData, gdMaterialSourceData);
//    
//    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_POSITION],
//                gdLightSourceData.Position.x, gdLightSourceData.Position.y, gdLightSourceData.Position.z);
//    glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_LIGHT0_DIRECTION],
//                gdLightSourceData.Direction.x, gdLightSourceData.Direction.y, gdLightSourceData.Direction.z);
    
    // Set master shader switch
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE], gdMasterShaderID);

    glDrawArrays(GL_TRIANGLES, 0, gdTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentDrawBuffer]);

    glBindVertexArrayOES(0);
    
    //
    // Draw sky
    //
    
    //glDisable(GL_DITHER);
    glDisable(GL_CULL_FACE);
    
    glBindVertexArrayOES(gdSkyTriangleVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, gdSkyTriangleVertexBuffer);

    //glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_DIFFUSE],
    //            gdSkyCube.color.x, gdSkyCube.color.y, gdSkyCube.color.z, gdSkyCube.color.w);
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE], gdSkyShaderID);
    
    gdSkyCube.centerPosition.x = gdPlayerData.position.x;
    gdSkyCube.centerPosition.z = gdPlayerData.position.z;
    IWCubeToTriangles(&gdSkyCube);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    gdSkyCube.triangleBufferData.size * sizeof(GLfloat), gdSkyCube.triangleBufferData.startCPU);
    
    gdSun.centerLocation.x = gdPlayerData.position.x;
    gdSun.centerLocation.z = gdPlayerData.position.z + 30.5;
    IWGCircleToTriangles(&gdSun);
    glBufferSubData(GL_ARRAY_BUFFER, gdSkyCube.triangleBufferData.size * sizeof(GLfloat),
                    gdSun.triangleBufferData.size * sizeof(GLfloat), gdSun.triangleBufferData.startCPU);
    
    glDrawArrays(GL_TRIANGLES, 0,
                 (gdSkyCube.triangleBufferData.size + gdSun.triangleBufferData.size) / gdSkyCube.triangleBufferData.stride);
    
    //glEnable(GL_DITHER);
    glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_MATERIAL_DIFFUSE],
                gdMaterialSourceData.Diffuse.x, gdMaterialSourceData.Diffuse.y, gdMaterialSourceData.Diffuse.z,
                gdMaterialSourceData.Diffuse.w);
    glBindVertexArrayOES(0);
    
    //
    // Draw text
    //
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    
    IWGMultiBufferBindCurrentDrawBuffer(&gdTextTriangleDoubleBuffer);
    
    // Set master shader switch
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
                5);
    
    glDrawArrays(GL_TRIANGLES, 0, gdTextTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    //
    // Draw user interface
    //
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    IWGMultiBufferBindCurrentDrawBuffer(&gdUITriangleDoubleBuffer);
    
    // Set master shader switch
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE], 0);
    
    //glUseProgram(_program);
    
    //glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
    //glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 1.0, 0.1, 0.1, 1.0);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT2 / 2);
    glDrawArrays(GL_TRIANGLES, 0, gdUINTriangleVertices);

    glBindVertexArrayOES(gdUILineVertexArray);

    glLineWidth(0.5);
    glDrawArrays(GL_LINES, 0, gdUINLineVertices);

    // Set master shader switch
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE], 2);
    
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