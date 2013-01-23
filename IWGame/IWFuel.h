//
//  IWFuel.h
//  theBr1dge
//
//  Created by Martin Raue on 1/22/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWFuel_h
#define theBr1dge_IWFuel_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWGeometry.h"
#include "IWUIElement.h"

struct _IWFuel {
    float currentLevel;
    float currentMaxLevel;
    float maxLevel;
    IWVector4 currentColor;
    IWVector4 currentMaxColor;
    IWVector4 maxColor;
    IWRectangle rectangle;
    IWUIElement uiElementData;
};

typedef struct _IWFuel IWFuel;

IWFuel IWFuelMake(float currentLevel,
                  float currentMaxLevel,
                  float maxLevel,
                  IWVector4 currentColor,
                  IWVector4 currentMaxColor,
                  IWVector4 maxColor,
                  IWRectangle rectangle,
                  IWUIElement uiElement);
IWFuel IWFuelMakeDefaultStart();

bool IWFuelAddFuel(IWFuel *fuel, float extraFuel);
bool IWFuelRemoveFuel(IWFuel *fuel, float extraFuel);
bool IWFuelExtendMaxLevel(IWFuel *fuel, float extraMaxLevel);

size_t IWFuelToTriangleBuffer(IWFuel *fuel, GLfloat* p);
size_t IWFuelToLineBuffer(IWFuel *fuel, GLfloat* p);

#endif
