//
//  IWScorePresenter.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWScorePresenter_h
#define ArchRunnerAlpha_IWScorePresenter_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

#include "IWScoreCounter.h"
#include "IWGTextField.h"
#include "IWGFontMap.h"

#include "IWGRingBuffer.h"

typedef struct {
    float timePerRow;
    float currentTime;
    bool hasFinished;
    IWGTextFieldData titleTextField;
    IWGTextFieldData descriptionTextField;
    IWGTextFieldData scoreTextField;
    size_t dataBufferSize;
    GLfloat* dataBufferStart;
    IWGRingBufferData multiBuffer;
} IWScorePresenterData;

IWScorePresenterData IWScorePresenterMake(float timePerRow,
                                          IWPoint2D anchorPoint,
                                          float aspect,
                                          float fontHeight,
                                          IWVector4 color,
                                          IWGFontMapData *fontMap);

void IWScorePresenterUpdate(IWScorePresenterData *scorePresenter,
                            IWScoreCounterData *scoreCounter,
                            float timeSinceLastUpdate);

void IWScorePresenterFillVBO(IWScorePresenterData *scorePresenter,
                             GLuint positionSlot,
                             GLuint colorSlot,
                             GLuint textureOffsetSlot,
                             GLuint textureHandlerId,
                             GLvoid* fontMapTextureData);

void IWScorePresenterRender(IWScorePresenterData *scorePresenter);

void IWScorePresenterPurgeData(IWScorePresenterData *scorePresenter);

#endif
