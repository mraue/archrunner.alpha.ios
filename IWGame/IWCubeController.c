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

IWCubeControllerData IWCubeControllerMake(int nx, int ny, int nz,
                                          float l, float d,
                                          IWVector3 center,
                                          IWVector4 color,
                                          unsigned int nRandomizePositions,
                                          float randomDistance,
                                          bool cubesInteractive,
                                          bool removeCubes,
                                          float removalTime,
                                          unsigned int nBridgeCubes)
{
    IWCubeControllerData cubeController;
    
    cubeController.nCubes = nx * ny * nz;
    
    cubeController.randomRemoveCubeTimer = IWTimerDataMake(0.0, removalTime, false);
    cubeController.removeCubes = removeCubes;
    
    // Allocate memory buffer
    cubeController.triangleDataBufferSize = cubeController.nCubes * 6 * 6 * 10;
    cubeController.triangleDataBufferStart = malloc(cubeController.triangleDataBufferSize * sizeof(GLfloat));
    
    cubeController.triangleRingBuffer = IWGRingBufferGen();
    
    cubeController.standardCubeIndexList = IWIndexListMake(cubeController.nCubes);
    cubeController.poolCubeIndexList = IWIndexListMake(cubeController.nCubes);
    cubeController.poolCubeIndexList.nEntries = 0;
    cubeController.gpuBufferPositionIndexList = IWIndexListMake(cubeController.nCubes);
    
    if (nBridgeCubes > cubeController.nCubes) {
        printf("ERROR: nBridgeCubes > cubeController.nCubes -> converting all cubes to bridge cubes\n");
        nBridgeCubes = cubeController.nCubes;
    }
    
    //srand(74803974122);// ok
    //unsigned int nCubesBridge = 114;
    //srand(111222333444);// default
    //unsigned int nCubesBridge = 140;
    //unsigned int nCubesBridge = 0;
    
    cubeController.cubeData = IWCubeMakeCubes(nx, ny, nz, l, d, center, color, nRandomizePositions, randomDistance);
    
    cubeController.secondaryPositions = IWCubeMakeCubeCurve(cubeController.nCubes,
                                                            IWVector3Make(0.0, 0.0, 0.0),
                                                            IWGEOMETRY_AXIS_Z);
    cubeController.secondaryPositionCounter = 0;
    
    GLfloat *memPtr = cubeController.triangleDataBufferStart;
    
    for (int nc=0; nc < cubeController.nCubes; nc++) {
        //
        cubeController.cubeData[nc].type = IWCUBE_TYPE_STANDARD;
        cubeController.cubeData[nc].isInteractive = cubesInteractive;
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

void IWCubeControllerRemoveCube(IWCubeControllerData *cubeController,
                                IWCubeData *cube)
{
    int bufferId = IWIndexListGetIndexForObjectId(&cubeController->gpuBufferPositionIndexList, cube->id);
    if (bufferId < 0) {
        printf("ERROR: Could not find cube with id %u in GPU buffer list\n", cube->id);
        return;
    }
    
    int newBufferId = IWIndexListReplaceWithLast(&cubeController->gpuBufferPositionIndexList, bufferId);
    
    cube->isInteractive = false;
    cube->type = IWCUBE_TYPE_POOL;
    
    //currentBufferID = cube->triangleBufferData.bufferIDGPU;
    if (bufferId != cubeController->gpuBufferPositionIndexList.nEntries) {
        int newCubeID = IWIndexListGetObjectIdForIndex(&cubeController->gpuBufferPositionIndexList, bufferId);
        if (newCubeID < 0) {
            printf("ERROR could not get object id from cubeController->GPUBufferPositionIndexList for buffer %i", bufferId);
            return;
        }
        
        if (newBufferId != bufferId) {
            cubeController->cubeData[newCubeID].triangleBufferData.bufferIDGPU = bufferId;
            
            IWGRingBufferSubData(&cubeController->triangleRingBuffer,
                                 cube->triangleBufferData.size * bufferId * sizeof(GLfloat),
                                 cube->triangleBufferData.size * sizeof(GLfloat),
                                 cubeController->cubeData[newCubeID].triangleBufferData.startCPU,
                                 true);
        }
    }
    cubeController->triangleRingBuffer.nVertices[cubeController->triangleRingBuffer.currentDataUpdateBuffer]
        -= cube->triangleBufferData.size / cube->triangleBufferData.stride;
    
    return;
}

void IWCubeControllerUpdate(IWCubeControllerData *cubeController,
                            const IWScoreCounterData *scoreCounter,
                            IWCubeStatusData *cubeStatus,
                            const IWFuel *fuel,
                            const IWPlayerData *player,
                            float timeSinceLastUpdate)
{
    // Auto remove standard cubes
    if (cubeController->removeCubes
        && cubeController->standardCubeIndexList.nEntries > 0
        && IWTimerUpdate(&cubeController->randomRemoveCubeTimer, timeSinceLastUpdate)) {
        
        IWTimerResetAndStart(&cubeController->randomRemoveCubeTimer);
        
        // Remove cube
        unsigned int i = IWIndexListRemoveRandom(&cubeController->standardCubeIndexList);
        //printf("DEBUG: Removing cube %u\n", i);
        IWCubeControllerRemoveCube(cubeController, &cubeController->cubeData[i]);
        
        cubeStatus->nGridCubes -= 1;
    }
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