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
#include "IWGCircle.h"

#include "IWGLighting.h"

#include "IWGBufferSubData.h"
#include "IWGMultiBuffer.h"

#include "IWGFontMap.h"
#include "IWGFontMapEntry.h"
#include "IWGTextField.h"

#include "IWScoreCounter.h"

#include "IWGSkyBox.h"

#include "IWUIMenuItem.h"
#include "IWUIMenuPage.h"
#include "IWUIMenuPresenter.h"
#include "IWUIMenu.h"

#include "IWGameData.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void IWGRendererSetupGL(const char* fontMapFilename)
{
    // Main lighting shader program
    IWGShaderProgramInitFromFiles(&gdMainShaderProgram);
    
    glUseProgram(gdMainShaderProgram.programID);
    
    // Get and save uniform locations.
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX]
        = glGetUniformLocation(gdMainShaderProgram.programID, "ModelMatrix");
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX]
        = glGetUniformLocation(gdMainShaderProgram.programID, "ViewMatrix");
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX]
        = glGetUniformLocation(gdMainShaderProgram.programID, "ProjectionMatrix");
    basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_NORMAL_MATRIX]
        = glGetUniformLocation(gdMainShaderProgram.programID, "NormalMatrix");
    
    GLuint grayScaleId = glGetUniformLocation(gdMainShaderProgram.programID, "GrayScale");
    
    glUniform2f(grayScaleId, 0.0, 0.4);
    
    gdSunLightSource = IWGBasicLightSourceMakeDefault();
    gdMoonLightSource = IWGBasicLightSourceMakeDefault();
    
    gdSunLightSource.Direction = IWVector3Normalize(IWVector3Make(0.0, 0.0, 1.0));
    
    gdMoonLightSource.Direction = IWVector3Normalize(IWVector3Make(3.0, 2.5, -2.0));
    gdMoonLightSource.Color = IWVector4Make(1.0, 1.0, 1.0, 1.0);
    
    gdPLayerLightSource = IWGPointLightSourceMakeDefault();
    
    IWGLightingInitializeUniformLocations(gdMainShaderProgram.programID);
    IWGLightingSetUniforms(&gdSunLightSource, &gdMoonLightSource, &gdPLayerLightSource);
    
    IWGShaderProgramInitFromFiles(&gdTextShaderProgram);
    
    IWGShaderProgramInitFromFiles(&gdUIShaderProgram);
    
    IWGShaderProgramInitFromFiles(&gdSkyboxShaderProgram);
    
    glUseProgram(gdSkyboxShaderProgram.programID);
    
    grayScaleId = glGetUniformLocation(gdSkyboxShaderProgram.programID, "GrayScale");
    
    glUniform2f(grayScaleId, 0.0, 0.4);
    
    // Get and save uniform locations.
    skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX]
        = glGetUniformLocation(gdSkyboxShaderProgram.programID, "ModelMatrix");
    skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX]
        = glGetUniformLocation(gdSkyboxShaderProgram.programID, "ViewMatrix");
    skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX]
        = glGetUniformLocation(gdSkyboxShaderProgram.programID, "ProjectionMatrix");
    
    gdFontMap = IWGFontMapCreateFromFile(fontMapFilename);
    
    gdTriangleDoubleBuffer = IWGMultiBufferGen();
    gdTextTriangleDoubleBuffer = IWGMultiBufferGen();
    gdUITriangleDoubleBuffer = IWGMultiBufferGen();
    
    // Could swith to multi buffer, ey!
    glGenVertexArraysOES(1, &gdUILineVertexArray);
    glBindVertexArrayOES(gdUILineVertexArray);
    glGenBuffers(1, &gdUILineVertexBuffer);
    glBindVertexArrayOES(0);
    
    // Open GL base settings
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glLineWidth(1.2);
    
    IWGRendererSetupStartMenuAssets();

    return;
}

void IWGRendererSetupStartMenuAssets(void)
{
    gdTriangleDoubleBuffer = IWGMultiBufferGen();
    gdTextTriangleDoubleBuffer = IWGMultiBufferGen();
    
    gdPlayerData = gdPlayerDataStart = IWPlayerDataMakeSimple(IWVector3Make(-0.8, 0.0, -1.),
                                                              IWVector3Normalize(IWVector3Make(0.4, 0.0, 1.0)),
                                                              IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0)));
    
    gdCurrentGameStatus = IWGAME_STATUS_START_MENU;
    
    gdClearColor = IWVector4Make(0.6, 0.6, 0.6, 1.0);
    glClearColor(gdClearColor.x, gdClearColor.y, gdClearColor.z, gdClearColor.w);
    
    int nx, ny, nz;
    nx = ny = nz = 8;
    
    int n = nx * ny * nz;
    size_t mypos_size = n * 6 * 6 * 10 * sizeof(GLfloat);

    gdCubeTriangleBufferStartCPU = malloc(mypos_size);
    
    gdStandardCubeIndexList = IWIndexListMake(n);
    gdPoolCubeIndexList = IWIndexListMake(n);
    gdPoolCubeIndexList.nEntries = 0;
    gdGPUBufferPositionIndexList = IWIndexListMake(n);
    
    IWVector4 cubeBaseColor = {0.5, 0.5, 0.5, 1.0};

    gdCubeData = IWCubeMakeCubes(nx, ny, nz, .04, .12, IWVector3Make(0.0, 0.0, 0.0), cubeBaseColor, 1, 0.05);
    gdNCubes = nx * ny * nz;
    
    gdSecondaryPosition = IWCubeMakeCubeCurve(gdNCubes, IWVector3Make(0.0, 0.0, 0.0), IWGEOMETRY_AXIS_Z);
    gdSecondaryPositionCounter = 0;
    
    GLfloat *memPtr = gdCubeTriangleBufferStartCPU;
    for (int nc=0; nc < n; nc++) {
        // Spawn cube
        gdCubeData[nc].type = IWCUBE_TYPE_STANDARD;
        gdCubeData[nc].isInteractive = false;

        //
        gdCubeData[nc].triangleBufferData.bufferStartCPU = gdCubeTriangleBufferStartCPU;
        gdCubeData[nc].triangleBufferData.startCPU = memPtr;
        gdCubeData[nc].triangleBufferData.bufferIDGPU = nc;
        //
        //gdCubeData[nc].faces = IWCUBE_FACES_BOWL;
        //
        gdCubeData[nc].triangleBufferData.bufferOffsetGPU = memPtr - gdCubeTriangleBufferStartCPU;
        //
        memPtr += IWCubeToTriangles(&gdCubeData[nc]);
        // Setup primitive data buffer chain
        //gdStandardCubeIndexList.map[nc] = nc;
        //gdStandardCubeIndexList.reverseMap[nc] = nc;
        IWIndexListAppendObjectId(&gdStandardCubeIndexList, nc);
        IWIndexListAppendObjectId(&gdGPUBufferPositionIndexList, nc);
    }
    
    unsigned int nVertices = (memPtr - gdCubeTriangleBufferStartCPU) / gdCubeData[0].triangleBufferData.stride;
    
    // Get attribute locations
    GLuint positionSlot = glGetAttribLocation(gdMainShaderProgram.programID, "Vertex");
    GLuint normalSlot = glGetAttribLocation(gdMainShaderProgram.programID, "Normal");
    GLuint colorSlot = glGetAttribLocation(gdMainShaderProgram.programID, "Color");
    GLuint textureOffsetSlot = glGetAttribLocation(gdMainShaderProgram.programID, "TextureOffset");
    
    for (unsigned int  k =0; k < IWGMULTIBUFFER_MAX; k++) {
        gdTriangleDoubleBuffer.nVertices[k] = nVertices;
    }
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        IWGMultiBufferBind(&gdTriangleDoubleBuffer, i);
        
        glBufferData(GL_ARRAY_BUFFER, mypos_size, gdCubeTriangleBufferStartCPU, GL_DYNAMIC_DRAW);
        
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
    
    glUseProgram(gdSkyboxShaderProgram.programID);
    
    positionSlot = glGetAttribLocation(gdSkyboxShaderProgram.programID, "Vertex");
    normalSlot = glGetAttribLocation(gdSkyboxShaderProgram.programID, "Normal");
    colorSlot = glGetAttribLocation(gdSkyboxShaderProgram.programID, "Color");
    
    gdSkyBox = IWGSkyBoxMakeDefault();
    IWGSkyBoxFillVBO(&gdSkyBox, positionSlot, colorSlot, normalSlot);

    //
    // Text
    //
    
    float aspect = fabsf(gdScreenWidth / gdScreenHeight);
    
    GLuint textureHandlerId;
    glGenTextures(1, &textureHandlerId);

    gdInGameTextTriangleBufferStartCPU = malloc(((2 * 10 + 1 * 9 + 1 * 10) * 6 * 9) * sizeof(GLfloat));
    
    gdTitleTextField = IWGTextFieldMake(IWVector2Make(0.95, 0.96),
                                        IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                                        2, 10,
                                        1. / aspect,
                                        "ARCHRUNNER\nALPHA",
                                        0.18, -0.04,
                                        IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                        IWVector4Make(1.0, 1.0, 1.0, 1.0),
                                        &gdFontMap,
                                        gdInGameTextTriangleBufferStartCPU);
    
    gdVersionTextField = IWGTextFieldMake(IWVector2Make(0.98, -0.99),
                                          IWGEOMETRY_ANCHOR_POSITION_LOWER_RIGHT,
                                          1, 9,
                                          1. / aspect,
                                          VERSION,
                                          0.1, 0.0,
                                          IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                          IWVector4Make(1.0, 1.0, 1.0, 1.0),
                                          &gdFontMap,
                                          gdInGameTextTriangleBufferStartCPU
                                          + gdTitleTextField.triangleBufferData.size);
    gdStartTextField = IWGTextFieldMake(IWVector2Make(0.85, 0.0),
                                          IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                                          1, 10,
                                          1. / aspect,
                                          "[START]",
                                          0.18, 0.0,
                                          IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                          IWVector4Make(1.0, 1.0, 1.0, 1.0),
                                          &gdFontMap,
                                          gdInGameTextTriangleBufferStartCPU
                                          + gdTitleTextField.triangleBufferData.size
                                        + gdVersionTextField.triangleBufferData.size);
    
    gdStartTextFieldColorTransition = IWVector4TransitionMake(IWVector4Make(1.0, 1.0, 1.0, 0.4),
                                                              IWVector4Make(1.0, 1.0, 1.0, 1.0),
                                                              IWVector4Make(1.0, 1.0, 1.0, 1.0),
                                                              1.0, 0.0, false, false);
    
    //gdInGameTextTriangleBufferStartCPU = gdTitleTextField.triangleBufferData.bufferStartCPU;
    
    // DEBUG
    // END DEBUG
    
    unsigned int textSizeTotal = gdTitleTextField.triangleBufferData.size + gdVersionTextField.triangleBufferData.size + gdStartTextField.triangleBufferData.size + gdMenuPresenterTest.triangleBufferData.size;
    
    for (unsigned int  k =0; k < IWGMULTIBUFFER_MAX; k++) {
        gdTextTriangleDoubleBuffer.nVertices[k] = textSizeTotal / gdTitleTextField.triangleBufferData.stride;
    }
    
    glUseProgram(gdTextShaderProgram.programID);
    
    positionSlot = glGetAttribLocation(gdTextShaderProgram.programID, "Vertex");
    colorSlot = glGetAttribLocation(gdTextShaderProgram.programID, "Color");
    textureOffsetSlot = glGetAttribLocation(gdTextShaderProgram.programID, "TextureOffset");
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        IWGMultiBufferBind(&gdTextTriangleDoubleBuffer, i);
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, gdFontMapTextureData);
        //glGenerateMipmap(GL_TEXTURE_2D);
        
        glBufferData(GL_ARRAY_BUFFER,
                     textSizeTotal* sizeof(GLfloat),
                     gdInGameTextTriangleBufferStartCPU,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textureOffsetSlot);
        glVertexAttribPointer(textureOffsetSlot, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    
    glBindVertexArrayOES(0);
    
    return;
}

void IWGRendererTearDownStartMenuAssets(void)
{
    free(gdCubeData);
    gdCubeData = NULL;
    
    free(gdCubeTriangleBufferStartCPU);
    gdCubeTriangleBufferStartCPU = NULL;

    IWGSkyBoxPurgeData(&gdSkyBox);
    
    IWGMultiBufferDealloc(&gdTextTriangleDoubleBuffer);
    IWGMultiBufferDealloc(&gdTriangleDoubleBuffer);
    
    glDeleteTextures(1, &gdTextureHandlerId);
    
    free(gdInGameTextTriangleBufferStartCPU);
    gdInGameTextTriangleBufferStartCPU = NULL;
    
    free(gdSecondaryPosition);
    gdSecondaryPosition = NULL;
    
    IWIndexListDeallocData(&gdStandardCubeIndexList);
    IWIndexListDeallocData(&gdPoolCubeIndexList);
    IWIndexListDeallocData(&gdGPUBufferPositionIndexList);
    
    IWUIStateBarDeallocData(&gdFuel.stateBar);
}

void IWGRendererSetupGameAssets(void)
{
    gdTriangleDoubleBuffer = IWGMultiBufferGen();
    gdTextTriangleDoubleBuffer = IWGMultiBufferGen();
    gdUITriangleDoubleBuffer = IWGMultiBufferGen();
    
    // Could swith to multi buffer, ey!
    glGenVertexArraysOES(1, &gdUILineVertexArray);
    glBindVertexArrayOES(gdUILineVertexArray);
    glGenBuffers(1, &gdUILineVertexBuffer);
    glBindVertexArrayOES(0);
    
    IWGameReset();
    
    gdClearColor = IWVector4Make(0.6, 0.6, 0.6, 1.0);
    //gdClearColor = IWVector4Make(0.95, 0.95, 0.95, 1.0);
    glClearColor(gdClearColor.x, gdClearColor.y, gdClearColor.z, gdClearColor.w);
    
    IWColorTransition overdriveColorTransition = {
        IWUI_COLOR_WHITE(0.4),
        IWUI_COLOR_WHITE(1.0),
        IWUI_COLOR_WHITE(0.4),
        0.4, 0.0, true, true
    };
    gdOverdriveColorTransition = overdriveColorTransition;
    
    int nx, ny, nz;
    nx = ny = nz = gdNCubesPerAxis;
    
    int n = nx * ny * nz;
    size_t mypos_size = n * 6 * 6 * 10 * sizeof(GLfloat);

    gdCubeTriangleBufferStartCPU = malloc(mypos_size);

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
    
    GLfloat *memPtr = gdCubeTriangleBufferStartCPU;
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
        float transitionTime = 2.;
        
        gdCubeData[nc].positionTransition = IWVector3TransitionMake(spawnPosition,
                                                                    gdCubeData[nc].centerPosition,
                                                                    spawnPosition,
                                                                    transitionTime, 0.0, false, false);
        gdCubeData[nc].centerPosition = spawnPosition;
        //gdCubeData[nc].color = IWUI_COLOR_DARK_BLUE(1.0);
        gdCubeData[nc].color = IWVector4Make(0.5, 0.5, 0.5, 1.0);
        //
        gdCubeData[nc].triangleBufferData.bufferStartCPU = gdCubeTriangleBufferStartCPU;
        gdCubeData[nc].triangleBufferData.startCPU = memPtr;
        gdCubeData[nc].triangleBufferData.bufferIDGPU = nc;
        //
        //gdCubeData[nc].faces = IWCUBE_FACES_BOWL;
        //
        gdCubeData[nc].triangleBufferData.bufferOffsetGPU = memPtr - gdCubeTriangleBufferStartCPU;
        //
        memPtr += IWCubeToTriangles(&gdCubeData[nc]);
        // Setup primitive data buffer chain
        //gdStandardCubeIndexList.map[nc] = nc;
        //gdStandardCubeIndexList.reverseMap[nc] = nc;
        IWIndexListAppendObjectId(&gdStandardCubeIndexList, nc);
        IWIndexListAppendObjectId(&gdGPUBufferPositionIndexList, nc);
    }
    
    unsigned int nVertices = (memPtr - gdCubeTriangleBufferStartCPU) / gdCubeData[0].triangleBufferData.stride;
    
    // Get attribute locations
    GLuint positionSlot = glGetAttribLocation(gdMainShaderProgram.programID, "Vertex");
    GLuint normalSlot = glGetAttribLocation(gdMainShaderProgram.programID, "Normal");
    GLuint colorSlot = glGetAttribLocation(gdMainShaderProgram.programID, "Color");
    GLuint textureOffsetSlot = glGetAttribLocation(gdMainShaderProgram.programID, "TextureOffset");
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {

        IWGMultiBufferBind(&gdTriangleDoubleBuffer, i);

        glBufferData(GL_ARRAY_BUFFER, mypos_size, gdCubeTriangleBufferStartCPU, GL_DYNAMIC_DRAW);
        
        gdTriangleDoubleBuffer.nVertices[i] = nVertices;

        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(normalSlot);
        glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    
    glBindVertexArrayOES(0);
    
    //
    // Sky box
    //
    
    glUseProgram(gdSkyboxShaderProgram.programID);
    
    positionSlot = glGetAttribLocation(gdSkyboxShaderProgram.programID, "Vertex");
    normalSlot = glGetAttribLocation(gdSkyboxShaderProgram.programID, "Normal");
    colorSlot = glGetAttribLocation(gdSkyboxShaderProgram.programID, "Color");

    gdSkyBox = IWGSkyBoxMakeDefault();
    IWGSkyBoxFillVBO(&gdSkyBox, positionSlot, colorSlot, normalSlot);
    
    //
    // Text
    //
    
    float aspect = fabsf(gdScreenWidth / gdScreenHeight);
    
    glGenTextures(1, &gdTextureHandlerId);

    gdInGameTextTriangleBufferStartCPU = malloc((1 * 10 + 3 * 10) * 6 * 9 * sizeof(GLfloat));

    gdScoreTextField = IWGTextFieldMake(IWVector2Make(0.95, 1.0),
                                        IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                                        1, 10,
                                        1. / aspect,
                                        "",
                                        0.4, -0.04,
                                        IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                        IWVector4Make(1.0, 1.0, 1.0, 0.7),
                                        &gdFontMap,
                                        gdInGameTextTriangleBufferStartCPU);
    
    gdGameStatusField = IWGTextFieldMake(IWVector2Make(0.93, 0.62),
                                         IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT,
                                         3, 10,
                                         1. / aspect,
                                         "",
                                         0.1, -0.01,
                                         IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                         IWVector4Make(1.0, 1.0, 1.0, 0.5),
                                         &gdFontMap,
                                         gdInGameTextTriangleBufferStartCPU
                                         + gdScoreTextField.triangleBufferData.size);
    
    
    unsigned int totalTextFieldsSize =
        gdScoreTextField.triangleBufferData.size
        + gdGameStatusField.triangleBufferData.size;
    
    gdStartTextFieldColorTransition = IWVector4TransitionMake(IWVector4Make(0.2, 0.2, 0.2, 0.8),
                                                              IWVector4Make(0.2, 0.2, 0.2, 0.5),
                                                              IWVector4Make(0.2, 0.2, 0.2, 0.8),
                                                              1.0, 0.0, false, false);
    
    glUseProgram(gdTextShaderProgram.programID);
    
    positionSlot = glGetAttribLocation(gdTextShaderProgram.programID, "Vertex");
    colorSlot = glGetAttribLocation(gdTextShaderProgram.programID, "Color");
    textureOffsetSlot = glGetAttribLocation(gdTextShaderProgram.programID, "TextureOffset");
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {

        gdTextTriangleDoubleBuffer.nVertices[i] = totalTextFieldsSize / gdScoreTextField.triangleBufferData.stride;
        
        IWGMultiBufferBind(&gdTextTriangleDoubleBuffer, i);
        
        glBindTexture(GL_TEXTURE_2D, gdTextureHandlerId);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, gdFontMapTextureData);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, gdFontMapTextureData);
        //glGenerateMipmap(GL_TEXTURE_2D);

        glBufferData(GL_ARRAY_BUFFER,
                     totalTextFieldsSize * sizeof(GLfloat),
                     gdInGameTextTriangleBufferStartCPU,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textureOffsetSlot);
        glVertexAttribPointer(textureOffsetSlot, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    
    glBindVertexArrayOES(0);
    
    // PAUSE Menu
    
    gdPauseMenu = IWUIMenuMake(IWUIMenuPresenterMake(2, 12, 1,
                                                    IWVector2Make(-0.4, 0.6), 1. / aspect,
                                                    0.22, 1.2,
                                                    IWVector4Make(0.2, 0.2, 0.2, 0.8),
                                                    &gdFontMap),
                              2);
    gdPauseMenu.pages[0].title = "PAUSED";
    gdPauseMenu.pages[0].isActive = true;
    //IWUIMenuPageAddItem(&gdPauseMenu.pages[0], IWUIMENUITEM_ITEM_TYPE_EMPTY, "", "", 0, NULL, 0);
    IWUIMenuPageAddItem(&gdPauseMenu.pages[0], IWUIMENUITEM_ITEM_TYPE_ACTION, "[QUIT]", "", 0, NULL, 1);
    //IWUIMenuPageAddItem(&gdPauseMenu.pages[0], IWUIMENUITEM_ITEM_TYPE_EMPTY, "", "", 0, NULL, 0);
    IWUIMenuPageAddItem(&gdPauseMenu.pages[0], IWUIMENUITEM_ITEM_TYPE_ACTION, "[CONTINUE]", "", 0, NULL, 2);
    gdPauseMenu.pages[1].title = "GAME OVER";
    gdPauseMenu.pages[1].isActive = false;
    IWUIMenuPageAddItem(&gdPauseMenu.pages[1], IWUIMENUITEM_ITEM_TYPE_ACTION, "[QUIT]", "", 0, NULL, 1);
    IWUIMenuPageAddItem(&gdPauseMenu.pages[1], IWUIMENUITEM_ITEM_TYPE_ACTION, "[RETRY]", "", 0, NULL, 2);
    
    IWUIMenuPresenterInitTextFields(&gdPauseMenu.presenter, gdPauseMenu.dataBufferStart);
    IWIUMenuPresenterPresentMenu(&gdPauseMenu.presenter, &gdPauseMenu.pages[0]);
    
    IWUIMenuFillVBO(&gdPauseMenu, positionSlot, colorSlot, textureOffsetSlot, gdTextureHandlerId, gdFontMapTextureData);
    
    //glUseProgram(gdMainShaderProgram.programID);
    
    //
    // Head up display data
    //
    
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

    //gdFuel.stateBar.direction = IWUI_DIRECTION_REVERSE;// need larger buffer! -> 4 x 6

    gdUINTriangleVertices = ((IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton)
                             + IWUIRectangleButtonTriangleBufferSize(&gdRectangleButton2)) / 7
                             + 6 * 3// 4 for reverse, 3 for normal
                             //+ 6 * 4// cube counter
                             );
    
    size_t mypos_size2 = gdUINTriangleVertices * 7 * sizeof(GLfloat);
    gdInGameUITriangleBufferStartCPU = malloc(mypos_size2);
    
    gdRectangleButton.triangleBuffer.bufferOffsetGPU = 0;    
    size_t offset = IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton, gdInGameUITriangleBufferStartCPU);
    gdRectangleButton2.triangleBuffer.bufferOffsetGPU = offset;
    offset += IWUIRectangleButtonToTriangleBuffer(&gdRectangleButton2, gdInGameUITriangleBufferStartCPU + offset);
    
    gdFuel.stateBar.triangleBufferData.bufferStartCPU = gdInGameUITriangleBufferStartCPU + offset;
    gdFuel.stateBar.triangleBufferData.bufferOffsetGPU = offset;
    gdFuel.stateBar.triangleBufferData.size = IWFuelToTriangleBuffer(&gdFuel, gdInGameUITriangleBufferStartCPU + offset);
    offset += gdFuel.stateBar.triangleBufferData.size;
    
    glUseProgram(gdUIShaderProgram.programID);
    
    positionSlot = glGetAttribLocation(gdUIShaderProgram.programID, "Vertex");
    colorSlot = glGetAttribLocation(gdUIShaderProgram.programID, "Color");

    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        IWGMultiBufferBind(&gdUITriangleDoubleBuffer, i);
        glBufferData(GL_ARRAY_BUFFER, mypos_size2, gdInGameUITriangleBufferStartCPU, GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    }
    
    glBindVertexArrayOES(0);
    
    glBindVertexArrayOES(gdUILineVertexArray);
    
    gdUINLineVertices = (IWUIRectangleButtonLineBufferSize(&gdRectangleButton)
                         + IWUIRectangleButtonLineBufferSize(&gdRectangleButton2)
                         + 2 * 52 * 7) / 7;
    
    mypos_size2 = gdUINLineVertices * 7 * sizeof(GLfloat);
    gdInGameUILineBufferStartCPU = malloc(mypos_size2);
    
    gdRectangleButton.lineBuffer.bufferOffsetGPU = 0;
    offset = IWUIRectangleButtonToLineBuffer(&gdRectangleButton, gdInGameUILineBufferStartCPU);
    gdRectangleButton2.lineBuffer.bufferOffsetGPU = offset;
    offset += IWUIRectangleButtonToLineBuffer(&gdRectangleButton2, gdInGameUILineBufferStartCPU + offset);
    
    IWUIElementData uiCentralCircle = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.06,//0.05,//0.3,//0.03,
                                                            IWVector4Make(1.0, 1.0, 1.0, 0.3), aspect, 41,
                                                            gdInGameUILineBufferStartCPU + offset);
    offset += uiCentralCircle.lineBufferSize;
    
    IWUIElementData uiCentralCircle2 = IWUIElementMakeCircle(IWVector2Make(0.5, 0.5), 0.001,//0.01,//0.1,//0.005,
                                                             IWVector4Make(1.0, 1.0, 1.0, 0.4), aspect, 11,
                                                             gdInGameUILineBufferStartCPU + offset);
    offset += uiCentralCircle2.lineBufferSize;

    glBindBuffer(GL_ARRAY_BUFFER, gdUILineVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mypos_size2, gdInGameUILineBufferStartCPU, GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(colorSlot);
    glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));

    glBindVertexArrayOES(0);
    
    return;
}

void IWGRendererTearDownGameAssets(void)
{
    free(gdCubeData);
    gdCubeData = NULL;

    free(gdCubeTriangleBufferStartCPU);
    gdCubeTriangleBufferStartCPU = NULL;
    
    free(gdInGameTextTriangleBufferStartCPU);
    gdInGameTextTriangleBufferStartCPU = NULL;
    
    free(gdInGameUILineBufferStartCPU);
    gdInGameUILineBufferStartCPU = NULL;
    
    free(gdInGameUITriangleBufferStartCPU);
    gdInGameUITriangleBufferStartCPU = NULL;
    
    IWGSkyBoxPurgeData(&gdSkyBox);
    
    glDeleteTextures(1, &gdTextureHandlerId);
    
    IWGMultiBufferDealloc(&gdTextTriangleDoubleBuffer);
    IWGMultiBufferDealloc(&gdTriangleDoubleBuffer);
    IWGMultiBufferDealloc(&gdUITriangleDoubleBuffer);
    
    glDeleteBuffers(1, &gdUILineVertexBuffer);
    glDeleteVertexArraysOES(1, &gdUILineVertexArray);
    
    gdUINLineVertices = 0;
    
    free(gdInGameTextTriangleBufferStartCPU);
    gdInGameTextTriangleBufferStartCPU = NULL;
    
    free(gdSecondaryPosition);
    gdSecondaryPosition = NULL;
    
    IWIndexListDeallocData(&gdStandardCubeIndexList);
    IWIndexListDeallocData(&gdPoolCubeIndexList);
    IWIndexListDeallocData(&gdGPUBufferPositionIndexList);
    
    IWUIStateBarDeallocData(&gdFuel.stateBar);
    
    IWUIMenuPurgeData(&gdPauseMenu);
}

void IWGRendererRenderCubes(void)
{
    // Render cubes
    
    IWGMultiBufferBindCurrentDrawBuffer(&gdTriangleDoubleBuffer);
    
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX],
                       1, 0, &gdModelMatrix.m00);
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX],
                       1, 0, &gdViewMatrix.m00);
    glUniformMatrix4fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX],
                       1, 0, &gdProjectionMatrix.m00);
    glUniformMatrix3fv(basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_NORMAL_MATRIX],
                       1, 0, &gdNormalMatrix.m00);
    
    glDrawArrays(GL_TRIANGLES, 0, gdTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
}


void IWGRendererRenderInGameUI(void)
{
    // Draw in game user interface
    
    IWGMultiBufferBindCurrentDrawBuffer(&gdUITriangleDoubleBuffer);
    
    // Set master shader switch
    //glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE], 0);
    
    //glUseProgram(_program);
    
    //glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
    //glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 1.0, 0.1, 0.1, 1.0);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT2 / 2);
    glDrawArrays(GL_TRIANGLES, 0, gdUINTriangleVertices);
    glBindVertexArrayOES(0);
    
    if (gdUINLineVertices) {
        glBindVertexArrayOES(gdUILineVertexArray);
        glDrawArrays(GL_LINES, 0, gdUINLineVertices);
        glBindVertexArrayOES(0);
    }
    
}

void IWGRendererRenderInGameText(void)
{
    // Draw in game text
    
    IWGMultiBufferBindCurrentDrawBuffer(&gdTextTriangleDoubleBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, gdTextTriangleDoubleBuffer.nVertices[gdTriangleDoubleBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
}

void IWGRendererRender(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (gdCurrentGameStatus == IWGAME_STATUS_RUNNING
        || gdCurrentGameStatus == IWGAME_STATUS_PAUSED
        || gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER) {
        
        glUseProgram(gdMainShaderProgram.programID);
        
        glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SUN_COLOR],
                    gdSkyBox.sunColorLight.x, gdSkyBox.sunColorLight.y, gdSkyBox.sunColorLight.z,
                    gdSkyBox.sunColorLight.w);
        
        float tmp = gdSkyBox.transitionTime / (gdSkyBox.colorTransitionTime * 1.2);
        tmp *= tmp;
        glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_COLOR],
                    0.75, 0.75, 0.75,
                    IW_MIN(1.0, tmp));
        
        glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_POSITION],
                    gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z);
        glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_DIRECTION],
                    gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z);
        
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        IWGRendererRenderCubes();

        glDisable(GL_CULL_FACE);
        
        glUseProgram(gdSkyboxShaderProgram.programID);
        
        glUniformMatrix4fv(skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX],
                           1, 0, &gdModelMatrix.m00);
        glUniformMatrix4fv(skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX],
                           1, 0, &gdViewMatrix.m00);
        glUniformMatrix4fv(skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX],
                           1, 0, &gdProjectionMatrix.m00);
        

        IWGSkyBoxRender(&gdSkyBox, false);

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);

        glUseProgram(gdTextShaderProgram.programID);

        if (gdCurrentGameStatus == IWGAME_STATUS_RUNNING)
            IWGRendererRenderInGameText();
        if (gdCurrentGameStatus == IWGAME_STATUS_PAUSED
            || gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER)
            IWUIMenuRender(&gdPauseMenu);

        glUseProgram(gdUIShaderProgram.programID);
        
        if (gdCurrentGameStatus == IWGAME_STATUS_RUNNING)
            IWGRendererRenderInGameUI();

        glDisable(GL_BLEND);
        
    } else if (gdCurrentGameStatus == IWGAME_STATUS_START_MENU) {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        glUseProgram(gdMainShaderProgram.programID);
        
        glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SUN_COLOR],
                    gdSkyBox.sunColorLight.x, gdSkyBox.sunColorLight.y, gdSkyBox.sunColorLight.z,
                    gdSkyBox.sunColorLight.w);
        
        float tmp = gdSkyBox.transitionTime / (gdSkyBox.colorTransitionTime * 1.2);
        tmp *= tmp;
        glUniform4f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_COLOR],
                    0.7, 0.7, 0.7,
                    IW_MIN(1.0, tmp));
        
        glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_POSITION],
                    gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z);
        glUniform3f(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_PLAYERLIGHT_DIRECTION],
                    gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z);
        
        IWGRendererRenderCubes();
        
        glDisable(GL_CULL_FACE);
        
        glUseProgram(gdSkyboxShaderProgram.programID);
        
        glUniformMatrix4fv(skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX],
                           1, 0, &gdModelMatrix.m00);
        glUniformMatrix4fv(skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX],
                           1, 0, &gdViewMatrix.m00);
        glUniformMatrix4fv(skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX],
                           1, 0, &gdProjectionMatrix.m00);

        IWGSkyBoxRender(&gdSkyBox, false);
        
        glDisable(GL_DEPTH_TEST);
        
        glEnable(GL_BLEND);
        
        glUseProgram(gdTextShaderProgram.programID);
        
        IWGRendererRenderInGameText();
        
        glDisable(GL_BLEND);
    }
    
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