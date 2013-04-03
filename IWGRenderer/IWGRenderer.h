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

#include "IWOpenGL.h"

#include "IWGShaderProgram.h"
#include "IWGLighting.h"

#include "IWPlayer.h"
#include "IWGSkyBoxController.h"

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

extern IWGShaderProgramData shaderProgramData;
extern GLuint basicUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_N];
extern GLuint skyboxShaderUniformIDs[IWGRENDERER_BASIC_UNIFORM_ID_INDEX_N];
extern GLuint attributeIDs[IWGRENDERER_ATTRIBUTE_ID_INDEX_N];

#pragma mark - Functions

void IWGRendererInit(const char* fontMapString);
void IWGRendererSetupGL();

void IWGRendererSetupGameAssets(void);
void IWGRendererTearDownGameAssets(void);

void IWGRendererUpdateUniforms(IWPlayerData* player,
                               IWGSkyBoxControllerData* skyBoxController);

void IWGRendererRender(void);

void IWGRendererRenderCubes(void);

void IWGRendererTearDownGL(void);

#endif
