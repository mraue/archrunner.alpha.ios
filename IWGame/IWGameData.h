//
//  IWGameData.h
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGameData_h
#define theBr1dge_IWGameData_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <GLKit/GLKMath.h>

#include "IWGame.h"
#include "IWGLighting.h"
#include "IWPlayer.h"
#include "IWUIRectangleButton.h"
#include "IWCube.h"
#include "IWColorTransition.h"
#include "IWFuel.h"
#include "IWGMultiBuffer.h"
#include "IWGBufferSubData.h"
#include "IWTimer.h"
#include "IWIndexList.h"
#include "IWCubeCounter.h"
#include "IWGCircle.h"

#pragma mark - Game Logic Data

bool gdGameIsPaused;
float gdPauseTime;

IWPlayerData gdPlayerDataStart;
IWPlayerData gdPlayerData;
IWPlayerData gdPlayerDataSave;

IWFuel gdFuel;

IWCubeCounterData gdCubeCounter;

IWCubeData* gdCubeData;
unsigned int gdNCubes;

float gdTotalRunTime;

IWTimerData gdRandomRemoveCubeTimer;

IWIndexListData gdStandardCubeIndexList;
IWIndexListData gdPoolCubeIndexList;
IWIndexListData gdGPUBufferPositionIndexList;

//unsigned int gdNOverdriveCubes;
float gdZMax;

IWCubeData gdSkyCube;
IWGCircleData gdSun;

#pragma mark - Second VBO array scaffolding

IWGMultiBufferData gdTriangleDoubleBuffer2;
GLuint gdTriangleNVertices2;

#pragma mark - Debug / Testing

unsigned int gdSecondaryPositionCounter;
IWVector3 *gdSecondaryPosition;

IWColorTransition gdClearColorTransition;
IWColorTransition gdOverdriveColorTransition;

bool gdSpawnCubes;

#pragma mark - User Interface

bool gdIsTouched;
IWPoint gdTouchPoint;

IWUIRectangleButton gdRectangleButton;
IWUIRectangleButton gdRectangleButton2;
IWUIRectangleButton gdRectangleButton3;

unsigned int gdUINTriangleVertices;
unsigned int gdUINLineVertices;

bool gdResetControllerPosition;

bool gdDropCamera;

bool gdRunningInSimulator;

#pragma mark - Open GL Buffer

GLint gdMasterShaderID;

IWGMultiBufferData gdTriangleDoubleBuffer;

GLuint gdSkyTriangleVertexArray;
GLuint gdSkyTriangleVertexBuffer;

IWGMultiBufferData gdUITriangleDoubleBuffer;

GLuint gdUITriangleVertexBuffer;

GLuint gdUILineVertexArray;
GLuint gdUILineVertexBuffer;

#pragma mark - Open GL Vertex Transformation Matrices

GLKMatrix4 gdModelMatrix;
GLKMatrix4 gdViewMatrix;
GLKMatrix4 gdProjectionMatrix;
GLKMatrix3 gdNormalMatrix;

#pragma mark - Open GL Lightsource Data

IWGLightSource gdLightSourceData;
IWGMaterialSource gdMaterialSourceData;

IWVector4 gdClearColor;

#endif
