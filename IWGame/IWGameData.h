//
//  IWGameData.h
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGameData_h
#define theBr1dge_IWGameData_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include <GLKit/GLKMath.h>

#include "IWGLighting.h"
#include "IWPlayer.h"

IWPlayerData gdPlayerData;

GLuint gdN_VERT;

GLuint gdVertexArray;
GLuint gdVertexArray2;
GLuint gdVertexBuffer;
GLuint gdVertexBuffer2;

GLKMatrix4 gdModelMatrix;
GLKMatrix4 gdViewMatrix;
GLKMatrix4 gdProjectionMatrix;
GLKMatrix3 gdNormalMatrix;

IWGLightSource gdLightSourceData;
IWGMaterialSource gdMaterialSourceData;

#endif
