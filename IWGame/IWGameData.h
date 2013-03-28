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

#include "IWOpenGL.h"

//#include <GLKit/GLKMath.h>

#include "IWGame.h"
#include "IWGLighting.h"
#include "IWPlayer.h"
#include "IWUIRectangleButton.h"
#include "IWCube.h"
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
#include "IWSoundHandler.h"

#pragma mark - Game Logic Data

extern IWGAME_STATUS gdCurrentGameStatus;

extern float gdPauseTime;

extern IWPlayerData gdPlayerDataStart;
extern IWPlayerData gdPlayerData;
extern IWPlayerData gdPlayerDataSave;

extern IWFuel gdFuel;

extern IWScoreCounterData gdScoreCounter;
extern IWCubeStatusData gdCubeStatus;

extern IWCubeData* gdCubeData;
extern unsigned int gdNCubes;
extern unsigned int gdNCubesPerAxis;

extern float gdTotalRunTime;

extern IWTimerData gdRandomRemoveCubeTimer;

extern IWIndexListData gdStandardCubeIndexList;
extern IWIndexListData gdPoolCubeIndexList;
extern IWIndexListData gdGPUBufferPositionIndexList;

extern unsigned int gdSecondaryPositionCounter;
extern IWVector3 *gdSecondaryPosition;

//float gdZMax;

extern IWGSkyBoxControllerData gdSkyBoxController;

extern IWTimerData gdStateSwitchTimer;

extern IWVector3Transition gdGrayScaleTransition;
extern IWVector3Transition gdGrayScaleTransitionDefault;

extern bool gdPushScoreToLeaderboard;
extern bool gdUpdateAchievements, gdAchievementReportedWatchingTheSunset;

extern IWTutorialControllerData *gdTutorialController;
extern IWStartMenuControllerData *gdStartMenuController;

extern IWGAME_MODUS gdGameModus;

#pragma mark - User input

extern IWControllerData gdControllerDataAccelerometer;

#pragma mark - Debug / Testing

//IWUIMenuPresenterData gdMenuPresenterTest;

extern int gdScreenShotUIMode;
extern IWVector3 gdScreenShotRotation;
extern IWUISliderData gdScreenShotSliderX, gdScreenShotSliderY, gdScreenShotSliderZ, gdScreenShotSliderE;

extern IWUserInterfaceControllerData gdUserInterfaceController;

extern IWSoundHandlerData *gdSoundHandler;

#pragma mark - Asset and memory management

extern IWGFontMapData gdFontMap;
extern void *gdFontMapTextureData;

extern GLfloat *gdCubeTriangleBufferStartCPU;
extern GLfloat *gdInGameTextTriangleBufferStartCPU;

extern GLuint gdTextureHandlerId;

#pragma mark - User Interface

extern bool gdIsTouched;
extern IWPoint2D gdTouchPoint;

extern bool gdResetControllerPosition;

extern bool gdRunningInSimulator;

extern IWUIMenuControllerData gdPauseMenu;
extern IWUIMenuControllerData gdMenuController;
extern IWScorePresenterData gdScorePresenter;

#pragma mark - Open GL Buffer

extern float gdScreenWidth;
extern float gdScreenHeight;

extern IWGShaderProgramData gdMainShaderProgram;
extern IWGShaderProgramData gdTextShaderProgram;
extern IWGShaderProgramData gdSkyboxShaderProgram;
extern IWGShaderProgramData gdUIShaderProgram;

extern IWGRingBufferData gdTextTriangleDoubleBuffer;

extern IWGRingBufferData gdTriangleDoubleBuffer;

extern IWGTextFieldData gdTitleTextField;
extern IWGTextFieldData gdVersionTextField;
extern IWGTextFieldData gdStartTextField;

extern IWVector4Transition gdStartTextFieldColorTransition;

#pragma mark - Open GL Vertex Transformation Matrices

extern IWMatrix4 gdModelMatrix;
extern IWMatrix4 gdViewMatrix;
extern IWMatrix4 gdProjectionMatrix;
extern IWMatrix3 gdNormalMatrix;

#pragma mark - Open GL Lightsource Data

extern IWGBasicLightSourceData gdSunLightSource;
extern IWGBasicLightSourceData gdMoonLightSource;
extern IWGPointLightSourceData gdPLayerLightSource;

extern IWVector4 gdClearColor;

#endif
