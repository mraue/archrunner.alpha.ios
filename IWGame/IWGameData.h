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
#include "IWGCircle.h"
#include "IWGFontMap.h"
#include "IWScoreCounter.h"
#include "IWGTextField.h"

#pragma mark - Game Logic Data

IWGAME_STATUS gdCurrentGameStatus;

bool gdGameIsPaused;
float gdPauseTime;

IWPlayerData gdPlayerDataStart;
IWPlayerData gdPlayerData;
IWPlayerData gdPlayerDataSave;

IWFuel gdFuel;

IWScoreCounterData gdScoreCounter;

IWCubeData* gdCubeData;
unsigned int gdNCubes;
unsigned int gdNCubesPerAxis;

float gdTotalRunTime;

IWTimerData gdRandomRemoveCubeTimer;

IWIndexListData gdStandardCubeIndexList;
IWIndexListData gdPoolCubeIndexList;
IWIndexListData gdGPUBufferPositionIndexList;

unsigned int gdSecondaryPositionCounter;
IWVector3 *gdSecondaryPosition;

//unsigned int gdNOverdriveCubes;
float gdZMax;

IWCubeData gdSkyCube;
IWGCircleData gdSun;

#pragma mark - Debug / Testing

IWColorTransition gdClearColorTransition;
IWColorTransition gdOverdriveColorTransition;

#pragma mark - Asset and memory management

IWGFontMapData gdFontMap;
void *gdFontMapTextureData;

GLfloat *gdCubeTriangleBufferStartCPU;
GLfloat *gdSkyTriangleBufferStartCPU;
GLfloat *gdInGameTextTriangleBufferStartCPU;
GLfloat *gdInGameUITriangleBufferStartCPU;
GLfloat *gdInGameUILineBufferStartCPU;

#pragma mark - User Interface

bool gdIsTouched;
IWPoint2D gdTouchPoint;

IWUIRectangleButton gdRectangleButton;
IWUIRectangleButton gdRectangleButton2;

unsigned int gdUINTriangleVertices;
unsigned int gdUINLineVertices;

bool gdResetControllerPosition;

bool gdRunningInSimulator;

#pragma mark - Open GL Buffer

GLuint gdGLProgramID;

GLint gdMasterShaderID;
GLint gdSkyShaderID;

IWGMultiBufferData gdTextTriangleDoubleBuffer;

IWGMultiBufferData gdTriangleDoubleBuffer;

GLuint gdSkyTriangleVertexArray;
GLuint gdSkyTriangleVertexBuffer;

IWGMultiBufferData gdUITriangleDoubleBuffer;

GLuint gdUITriangleVertexBuffer;

GLuint gdUILineVertexArray;
GLuint gdUILineVertexBuffer;

IWGTextFieldData gdScoreTextField;

IWGTextFieldData gdTitleTextField;
IWGTextFieldData gdVersionTextField;
IWGTextFieldData gdStartTextField;

IWVector4Transition gdStartTextFieldColorTransition;

#pragma mark - Open GL Vertex Transformation Matrices

IWMatrix4 gdModelMatrix;
IWMatrix4 gdViewMatrix;
IWMatrix4 gdProjectionMatrix;
IWMatrix3 gdNormalMatrix;

#pragma mark - Open GL Lightsource Data

IWGLightSource gdLightSourceData;
IWGMaterialSource gdMaterialSourceData;

IWVector4 gdClearColor;

#endif
