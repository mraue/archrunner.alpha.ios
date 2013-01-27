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
#include "IWGDoubleBuffer.h"
#include "IWGBufferSubData.h"


#pragma mark - Game Logic Data

IWPlayerData gdPlayerData;
IWPlayerData gdPlayerDataSave;

IWFuel gdFuel;

IWCubeData* gdCubeData;
unsigned int gdNCubes;

float gdTotalRunTime;

#pragma mark - Second VBO array scaffolding

IWGDoubleBufferData gdTriangleDoubleBuffer2;
GLuint gdTriangleNVertices2;

#pragma mark - Debug / Testing

unsigned int *gdBufferToCubeMap;
unsigned int gdBufferToCubeMapNEntries;

IWColorTransition gdClearColorTransition;

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

#pragma mark - Open GL Buffer

IWGDoubleBufferData gdTriangleDoubleBuffer;

GLuint gdB1TriangleNVertices;
GLuint gdB2TriangleNVertices;

IWGDoubleBufferData gdUITriangleDoubleBuffer;

GLuint gdUITriangleVertexArray;
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

#endif
