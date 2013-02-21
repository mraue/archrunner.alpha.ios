//
//  IWGTextField.h
//  theBr1dge
//
//  Created by Martin Raue on 2/8/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGTextField_h
#define theBr1dge_IWGTextField_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWGeometry.h"
#include "IWGPrimitiveBuffer.h"
#include "IWGFontMap.h"

typedef enum {
    IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
    IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
    IWGTEXT_HORIZONTAL_ALIGNMENT_CENTER
} IWGTEXT_HORIZONTAL_ALIGNMENT;

typedef struct {
    IWPoint2D anchorPoint;
    IWGEOMETRY_ANCHOR_POSITION anchorPosition;
    unsigned int nRows;
    unsigned int nColumns;
    float aspect;
    const char* text;
    float fontHeight;
    float lineExtraOffset;
    IWGTEXT_HORIZONTAL_ALIGNMENT alignment;
    IWVector4 color;
    IWGFontMapData *fontMap;
    IWGPrimitiveBufferData triangleBufferData;
    bool manageBuffer;
} IWGTextFieldData;

IWGTextFieldData IWGTextFieldMake(IWPoint2D anchorPoint,
                                  IWGEOMETRY_ANCHOR_POSITION anchorPosition,
                                  unsigned int nRows,
                                  unsigned int nColumns,
                                  float aspect,
                                  const char* text,
                                  float fontHeight,
                                  float lineExtraOffset,
                                  IWGTEXT_HORIZONTAL_ALIGNMENT alignment,
                                  IWVector4 color,
                                  IWGFontMapData *fontMap,
                                  GLfloat *bufferStartPtr);// If NULL, create and manage own Buffer

void IWGTextFieldSetText(IWGTextFieldData *textField, const char* text);
float IWGTextFieldGetCharacterWidth(IWGTextFieldData *textField);

#endif
