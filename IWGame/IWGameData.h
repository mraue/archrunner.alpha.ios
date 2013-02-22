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
#include "IWController.h"
#include "IWGSkyBox.h"
#include "IWGShaderProgram.h"
#include "IWGameStatus.h"
#include "IWUIMenuPresenter.h"
#include "IWUIMenu.h"
#include "IWScorePresenter.h"

#pragma mark - Game Logic Data

IWGAME_STATUS gdCurrentGameStatus;

float gdPauseTime;

IWPlayerData gdPlayerDataStart;
IWPlayerData gdPlayerData;
IWPlayerData gdPlayerDataSave;

IWFuel gdFuel;

IWScoreCounterData gdScoreCounter;
IWGameStatusData gdGameStatus;

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

float gdZMax;

IWGSkyBoxData gdSkyBox;

IWTimerData gdStateSwitchTimer;

IWColorTransition gdOverdriveColorTransition;

IWVector3Transition gdGrayScaleTransition;
IWVector3Transition gdGrayScaleTransitionDefault;

bool gdPushScoreToLeaderboard;

#pragma mark - User input

IWControllerData gdControllerDataAccelerometer;

#pragma mark - Debug / Testing

IWUIMenuPresenterData gdMenuPresenterTest;
IWUIMenuData gdMenuTest;

IWScorePresenterData gdScorePresenterTest;

#pragma mark - Asset and memory management

IWGFontMapData gdFontMap;
void *gdFontMapTextureData;

GLfloat *gdCubeTriangleBufferStartCPU;
GLfloat *gdInGameTextTriangleBufferStartCPU;
GLfloat *gdInGameUITriangleBufferStartCPU;
GLfloat *gdInGameUILineBufferStartCPU;

GLuint gdTextureHandlerId;

#pragma mark - User Interface

bool gdIsTouched;
IWPoint2D gdTouchPoint;

IWUIRectangleButton gdRectangleButton;
IWUIRectangleButton gdRectangleButton2;

unsigned int gdUINTriangleVertices;
unsigned int gdUINLineVertices;

bool gdResetControllerPosition;

bool gdRunningInSimulator;

IWUIMenuData gdPauseMenu;

#pragma mark - Open GL Buffer

float gdScreenWidth;
float gdScreenHeight;

IWGShaderProgramData gdMainShaderProgram;
IWGShaderProgramData gdTextShaderProgram;
IWGShaderProgramData gdSkyboxShaderProgram;
IWGShaderProgramData gdUIShaderProgram;

IWGMultiBufferData gdTextTriangleDoubleBuffer;

IWGMultiBufferData gdTriangleDoubleBuffer;

IWGMultiBufferData gdUITriangleDoubleBuffer;

GLuint gdUITriangleVertexBuffer;

GLuint gdUILineVertexArray;
GLuint gdUILineVertexBuffer;

IWGTextFieldData gdScoreTextField;
IWGTextFieldData gdGameStatusField;

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

IWGBasicLightSourceData gdSunLightSource;
IWGBasicLightSourceData gdMoonLightSource;
IWGPointLightSourceData gdPLayerLightSource;

IWVector4 gdClearColor;

#endif
