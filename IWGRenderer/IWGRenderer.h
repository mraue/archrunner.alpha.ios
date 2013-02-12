//
//  IWGRenderer.h
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGRenderer_h
#define theBr1dge_IWGRenderer_h

#pragma mark - Includes

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWGShaderProgram.h"
#include "IWGLighting.h"

#pragma mark - Definitions

enum _IWGRENDERER_BASIC_UNIFORM_ID_INDEX {
    IWGRENDERER_BASIC_UNIFORM_ID_INDEX_MODEL_MATRIX,
    IWGRENDERER_BASIC_UNIFORM_ID_INDEX_VIEW_MATRIX,
    IWGRENDERER_BASIC_UNIFORM_ID_INDEX_PROJECTION_MATRIX,
    IWGRENDERER_BASIC_UNIFORM_ID_INDEX_NORMAL_MATRIX,
    IWGRENDERER_BASIC_UNIFORM_ID_INDEX_N
};
typedef enum _IWGRENDERER_BASIC_UNIFORM_ID_INDEX IWGRENDERER_BASIC_UNIFORM_ID_INDEX;

enum _IWGRENDERER_ATTRIBUTE_ID_INDEX {
    IWGRENDERER_ATTRIBUTE_ID_INDEX_VERTEX,
    IWGRENDERER_ATTRIBUTE_ID_INDEX_NORMAL,
    IWGRENDERER_ATTRIBUTE_ID_INDEX_COLOR,
    IWGRENDERER_ATTRIBUTE_ID_INDEX_N
};
typedef enum _IWGRENDERER_ATTRIBUTE_ID_INDEX IWGRENDERER_ATTRIBUTE_ID_INDEX;

#pragma mark - Variables

IWGShaderProgramData shaderProgramData;
GLuint basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_N];
GLuint attributeIDs[IWGRENDERER_ATTRIBUTE_ID_INDEX_N];

#pragma mark - Functions

void IWGRendererSetupGL(const char* vertexShaderFilename,
                        const char* fragmentShaderFilename,
                        const char* fontMapFilename);

void IWGRendererSetupStartMenuAssets(void);
void IWGRendererTearDownStartMenuAssets(void);

void IWGRendererSetupGameAssets(void);
void IWGRendererTearDownGameAssets(void);

void IWGRendererRender(void);

void IWGRendererRenderCubes(void);
void IWGRendererRenderInGameUI(void);
void IWGRendererRenderSkybox(void);
void IWGRendererRenderInGameText(void);

void IWGRendererTearDownGL(void);

#endif
