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

bool gdGameIsPaused;
float gdPauseTime;

IWPlayerData gdPlayerDataStart;
IWPlayerData gdPlayerData;
IWPlayerData gdPlayerDataSave;

IWFuel gdFuel;

IWScoreCounterData gdScoreCounter;

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

IWGFontMapData gdFontMap;
void *gdFontMapTextureData;

#pragma mark - User Interface

bool gdIsTouched;
IWPoint2D gdTouchPoint;

IWUIRectangleButton gdRectangleButton;
IWUIRectangleButton gdRectangleButton2;
IWUIRectangleButton gdRectangleButton3;

unsigned int gdUINTriangleVertices;
unsigned int gdUINLineVertices;

bool gdResetControllerPosition;

bool gdRunningInSimulator;

#pragma mark - Open GL Buffer

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
