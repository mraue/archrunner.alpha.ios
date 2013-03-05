//
//  IWCubeController.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWCubeController_h
#define ArchRunnerAlpha_IWCubeController_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWCube.h"
#include "IWGRingBuffer.h"
#include "IWScoreCounter.h"
#include "IWFuel.h"
#include "IWPlayer.h"
#include "IWCubeStatus.h"
#include "IWGShaderProgram.h"
#include "IWTimer.h"
#include "IWIndexList.h"

//IWCubeData* gdCubeData;
//unsigned int gdNCubes;
//unsigned int gdNCubesPerAxis;
//
//float gdTotalRunTime;
//
//IWTimerData gdRandomRemoveCubeTimer;
//
//IWIndexListData gdStandardCubeIndexList;
//IWIndexListData gdPoolCubeIndexList;
//IWIndexListData gdGPUBufferPositionIndexList;
//
//unsigned int gdSecondaryPositionCounter;
//IWVector3 *gdSecondaryPosition;

typedef struct {
    //
    IWCubeData* cubeData;
    unsigned int nCubes;
    unsigned int nCubesPerAxis;
    //
    bool randomizePosition;
    //
    bool removeCubes;
    IWTimerData randomRemoveCubeTimer;
    //
    IWIndexListData standardCubeIndexList;
    IWIndexListData poolCubeIndexList;
    IWIndexListData gpuBufferPositionIndexList;
    //
    IWVector3 *secondaryPositions;
    unsigned int secondaryPositionCounter;
    //
    size_t triangleDataBufferSize;
    GLfloat* triangleDataBufferStart;
    IWGRingBufferData triangleRingBuffer;
} IWCubeControllerData;

IWCubeControllerData IWCubeControllerMake(unsigned int nCubesPerAxis,
                                          bool removeCubes,
                                          float removalTime,
                                          bool randomizePosition,
                                          unsigned int nBridgeCubes);

void IWCubeControllerSetupVBOs(IWCubeControllerData *cubeController,
                               const IWGShaderProgramData *shaderProgram);

void IWCubeControllerUpdate(IWCubeControllerData *cubeController,
                            const IWScoreCounterData *scoreCounter,
                            const IWCubeStatusData *cubeStatus,
                            const IWFuel *fuel,
                            const IWPlayerData *player,
                            float timeSinceLastUpdate);

void IWCubeControllerRender(IWCubeControllerData *cubeController,
                            GLuint programId);

void IWCubeControllerPurgeData(IWCubeControllerData *cubeController);

#endif
