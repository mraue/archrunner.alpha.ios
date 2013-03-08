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
//#include "IWColorTransition.h"
#include "IWFuel.h"
#include "IWGRingBuffer.h"
#include "IWGBufferSubData.h"
#include "IWTimer.h"
#include "IWIndexList.h"
#include "IWGCircle.h"
#include "IWGFontMap.h"
#include "IWScoreCounter.h"
#include "IWGTextField.h"
#include "IWController.h"
#include "IWGSkyBoxController.h"
#include "IWGShaderProgram.h"
#include "IWCubeStatus.h"
#include "IWUIMenuPresenter.h"
#include "IWUIMenuController.h"
#include "IWScorePresenter.h"
#include "IWUISlider.h"
#include "IWUserInterfaceController.h"
#include "IWTutorialController.h"
#include "IWStartMenuController.h"

#pragma mark - Game Logic Data

IWGAME_STATUS gdCurrentGameStatus;

float gdPauseTime;

IWPlayerData gdPlayerDataStart;
IWPlayerData gdPlayerData;
IWPlayerData gdPlayerDataSave;

IWFuel gdFuel;

IWScoreCounterData gdScoreCounter;
IWCubeStatusData gdCubeStatus;

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

IWGSkyBoxControllerData gdSkyBoxController;

IWTimerData gdStateSwitchTimer;

//IWColorTransition gdOverdriveColorTransition;

IWVector3Transition gdGrayScaleTransition;
IWVector3Transition gdGrayScaleTransitionDefault;

bool gdPushScoreToLeaderboard;
bool gdUpdateAchievements;

IWTutorialControllerData *gdTutorialController;
IWStartMenuControllerData *gdStartMenuController;

#pragma mark - User input

IWControllerData gdControllerDataAccelerometer;

#pragma mark - Debug / Testing

IWUIMenuPresenterData gdMenuPresenterTest;
IWUIMenuControllerData gdMenuController;

IWScorePresenterData gdScorePresenterTest;

int gdScreenShotUIMode;
IWVector3 gdScreenShotRotation;
IWUISliderData gdScreenShotSliderX, gdScreenShotSliderY, gdScreenShotSliderZ, gdScreenShotSliderE;

IWUserInterfaceControllerData gdUserInterfaceController;

#pragma mark - Asset and memory management

IWGFontMapData gdFontMap;
void *gdFontMapTextureData;

GLfloat *gdCubeTriangleBufferStartCPU;
GLfloat *gdInGameTextTriangleBufferStartCPU;
//GLfloat *gdInGameUITriangleBufferStartCPU;
//GLfloat *gdInGameUILineBufferStartCPU;

GLuint gdTextureHandlerId;

#pragma mark - User Interface

bool gdIsTouched;
IWPoint2D gdTouchPoint;

bool gdResetControllerPosition;

bool gdRunningInSimulator;

IWUIMenuControllerData gdPauseMenu;

#pragma mark - Open GL Buffer

float gdScreenWidth;
float gdScreenHeight;

IWGShaderProgramData gdMainShaderProgram;
IWGShaderProgramData gdTextShaderProgram;
IWGShaderProgramData gdSkyboxShaderProgram;
IWGShaderProgramData gdUIShaderProgram;

IWGRingBufferData gdTextTriangleDoubleBuffer;

IWGRingBufferData gdTriangleDoubleBuffer;

//GLuint gdUITriangleVertexBuffer;

//GLuint gdUILineVertexArray;
//GLuint gdUILineVertexBuffer;

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
