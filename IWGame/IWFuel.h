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

#include "IWUserInterface.h"
#include "IWUIStateBar.h"

enum _IWFUEL_COLOR {
    IWFUEL_COLOR_CURRENT,
    IWFUEL_COLOR_CURRENTMAX,
    IWFUEL_COLOR_MAX
};
typedef enum _IWFUEL_COLOR IWFUEL_COLOR;

struct _IWFuel {
    float currentLevel;
    float currentMaxLevel;
    float maxLevel;
    float warningLevel;
    bool isWarning;
    IWVector4 currentColor;
    IWVector4 currentMaxColor;
    IWVector4 maxColor;
    IWVector4 warningColor;
};

typedef struct _IWFuel IWFuel;

IWFuel IWFuelMake(float currentLevel,
                  float currentMaxLevel,
                  float maxLevel,
                  float warningLevel,
                  IWVector4 currentColor,
                  IWVector4 currentMaxColor,
                  IWVector4 maxColor,
                  IWVector4 warningColor);
IWFuel IWFuelMakeDefaultStart();

bool IWFuelAddFuel(IWFuel *fuel, float extraFuel);
bool IWFuelRemoveFuel(IWFuel *fuel, float extraFuel);
bool IWFuelExtendMaxLevel(IWFuel *fuel, float extraMaxLevel);

void IWFuelToStateBar(IWFuel *fuel, IWUIStateBar* stateBar);

#endif
