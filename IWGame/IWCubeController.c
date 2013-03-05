//
//  IWCubeController.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWCubeController.h"

#include <stdio.h>
#include <stdlib.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWCubeControllerData IWCubeControllerMake(unsigned int nCubesPerAxis,
                                          bool removeCubes,
                                          float removalTime,
                                          bool randomizePosition,
                                          unsigned int nBridgeCubes)
{
    IWCubeControllerData cubeController;
    
    int nx, ny, nz;
    nx = ny = nz = nCubesPerAxis;// [8]
    
    cubeController.nCubes = nx * ny * nz;
    
    // Allocate memory buffer
    cubeController.triangleDataBufferSize = cubeController.nCubes * 6 * 6 * 10;
    cubeController.triangleDataBufferStart = malloc(cubeController.triangleDataBufferSize * sizeof(GLfloat));
    
    cubeController.triangleRingBuffer = IWGRingBufferGen();
    
    cubeController.standardCubeIndexList = IWIndexListMake(cubeController.nCubes);
    cubeController.poolCubeIndexList = IWIndexListMake(cubeController.nCubes);
    cubeController.poolCubeIndexList.nEntries = 0;
    cubeController.gpuBufferPositionIndexList = IWIndexListMake(cubeController.nCubes);
    
    IWVector4 cubeBaseColor = {0.5, 0.5, 0.5, 1.0};
    
    if (nBridgeCubes > cubeController.nCubes) {
        printf("ERROR: nBridgeCubes > cubeController.nCubes -> converting all cubes to bridge cubes\n");
        nBridgeCubes = cubeController.nCubes;
    }
    
    //srand(74803974122);// ok
    //unsigned int nCubesBridge = 114;
    //srand(111222333444);// default
    //unsigned int nCubesBridge = 140;
    //unsigned int nCubesBridge = 0;
    
    cubeController.cubeData = IWCubeMakeCubes(nx, ny, nz, .04, .12, IWVector3Make(0.0, 0.0, 0.0), cubeBaseColor,
                                              randomizePosition ? 1 : 0, 0.05);
    
    cubeController.secondaryPositions = IWCubeMakeCubeCurve(cubeController.nCubes,
                                                            IWVector3Make(0.0, 0.0, 0.0),
                                                            IWGEOMETRY_AXIS_Z);
    cubeController.secondaryPositionCounter = 0;
    
    GLfloat *memPtr = cubeController.triangleDataBufferStart;
    
    for (int nc=0; nc < cubeController.nCubes; nc++) {
        //
        cubeController.cubeData[nc].type = IWCUBE_TYPE_STANDARD;
        cubeController.cubeData[nc].isInteractive = false;
        if (nc > cubeController.nCubes  - nBridgeCubes) {//114
            cubeController.cubeData[nc].centerPosition
                = cubeController.secondaryPositions[cubeController.secondaryPositionCounter];
            cubeController.cubeData[nc].color = IWUI_COLOR_RED(1.0);
            cubeController.secondaryPositionCounter++;
        }
        //
        cubeController.cubeData[nc].triangleBufferData.bufferStartCPU = cubeController.triangleDataBufferStart;
        cubeController.cubeData[nc].triangleBufferData.startCPU = memPtr;
        cubeController.cubeData[nc].triangleBufferData.bufferIDGPU = nc;
        //
        //gdCubeData[nc].faces = IWCUBE_FACES_BOWL;
        //
        cubeController.cubeData[nc].triangleBufferData.bufferOffsetGPU = memPtr - cubeController.triangleDataBufferStart;
        //
        memPtr += IWCubeToTriangles(&cubeController.cubeData[nc]);
        // Setup primitive data buffer chain
        //gdStandardCubeIndexList.map[nc] = nc;
        //gdStandardCubeIndexList.reverseMap[nc] = nc;
        IWIndexListAppendObjectId(&cubeController.standardCubeIndexList, nc);
        IWIndexListAppendObjectId(&cubeController.gpuBufferPositionIndexList, nc);
    }
    
    return cubeController;
}

void IWCubeControllerSetupVBOs(IWCubeControllerData *cubeController,
                               const IWGShaderProgramData *shaderProgram)
{
    glUseProgram(shaderProgram->programID);
    
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        cubeController->triangleRingBuffer.nVertices[i] = cubeController->triangleDataBufferSize / 10;
        
        IWGRingBufferBind(&cubeController->triangleRingBuffer, i);
        
        glBufferData(GL_ARRAY_BUFFER,
                     cubeController->triangleDataBufferSize * sizeof(GLfloat),
                     cubeController->triangleDataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(shaderProgram->vertexSlot);
        glVertexAttribPointer(shaderProgram->vertexSlot, 3, GL_FLOAT, GL_FALSE,
                              10 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(shaderProgram->colorSlot);
        glVertexAttribPointer(shaderProgram->colorSlot, 4, GL_FLOAT, GL_FALSE,
                              10 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(shaderProgram->normalSlot);
        glVertexAttribPointer(shaderProgram->normalSlot, 3, GL_FLOAT, GL_FALSE,
                              10 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }

    glBindVertexArrayOES(0);
}

void IWCubeControllerUpdate(IWCubeControllerData *cubeController,
                            const IWScoreCounterData *scoreCounter,
                            const IWCubeStatusData *cubeStatus,
                            const IWFuel *fuel,
                            const IWPlayerData *player,
                            float timeSinceLastUpdate)
{
    
}

void IWCubeControllerRender(IWCubeControllerData *cubeController,
                            GLuint programId)
{
    glUseProgram(programId);
    
    IWGRingBufferBindCurrentDrawBuffer(&cubeController->triangleRingBuffer);
    
    glDrawArrays(GL_TRIANGLES,
                 0,
                 cubeController->triangleRingBuffer.nVertices[cubeController->triangleRingBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGRingBufferSwitchBuffer(&cubeController->triangleRingBuffer);
}

void IWCubeControllerPurgeData(IWCubeControllerData *cubeController)
{
    free(cubeController->cubeData);
    cubeController->cubeData = NULL;

    free(cubeController->triangleDataBufferStart);
    cubeController->triangleDataBufferStart = NULL;
    
    IWGRingBufferDealloc(&cubeController->triangleRingBuffer);
    
    free(cubeController->secondaryPositions);
    cubeController->secondaryPositions = NULL;
    
    IWIndexListDeallocData(&cubeController->standardCubeIndexList);
    IWIndexListDeallocData(&cubeController->poolCubeIndexList);
    IWIndexListDeallocData(&cubeController->gpuBufferPositionIndexList);
}