//
//  IWFuel.c
//  theBr1dge
//
//  Created by Martin Raue on 1/22/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWFuel.h"

IWFuel IWFuelMake(float currentLevel,
                  float currentMaxLevel,
                  float maxLevel,
                  float warningLevel,
                  IWVector4 currentColor,
                  IWVector4 currentMaxColor,
                  IWVector4 maxColor,
                  IWVector4 warningColor,
                  IWRectangle rectangle,
                  IWUIElementData uiElement)
{
    float states[] = {currentLevel / maxLevel, currentMaxLevel / maxLevel, 1.0};
    IWVector4 colors[] = { currentColor, currentMaxColor, maxColor};
    
    IWFuel fuel = {
        currentLevel, currentMaxLevel, maxLevel, warningLevel, false,
        currentColor, currentMaxColor, maxColor, warningColor,
        IWUIStateBarMake(3, states, colors, rectangle,
                         IWUI_ORIENTATION_HORIZONTAL, IWUI_DIRECTION_TO_UP)
    };
    
    return fuel;
}
IWFuel IWFuelMakeDefaultStart()
{
    //IWRectangle rectangle = {{0.97, 0.25}, {0.99, 0.75}};
    IWRectangle rectangle = {{0.01, 0.96}, {0.5, 0.99}};
    IWFuel fuel = IWFuelMake(0.6, 0.6, 1.0, 0.33,
                             IWUI_COLOR_LIGHT_BLUE(0.6), IWUI_COLOR_BLUE(0.4), IWUI_COLOR_DARK_BLUE(0.4),
                             IWVector4Make(255.0 / 255.0, 139.0 / 255.0, 139.0 / 255.0, 0.6),
                             rectangle,
                             IWUIElementMakeEmpty());
    return fuel;
}

bool IWFuelAddFuel(IWFuel *fuel, float extraFuel)
{
    if (fuel->currentLevel == fuel->currentMaxLevel)
        return false;
    
    float newFuelLevel = fuel->currentLevel + extraFuel;
    if (newFuelLevel > fuel->currentMaxLevel) {
        fuel->currentLevel = fuel->currentMaxLevel;
        fuel->stateBar.states[0] = fuel->currentMaxLevel / fuel->maxLevel;
        return false;
    } else {
        fuel->currentLevel = newFuelLevel;
        fuel->stateBar.states[0] = newFuelLevel / fuel->maxLevel;
        return true;
    }
}

bool IWFuelRemoveFuel(IWFuel *fuel, float extraFuel)
{
    if (fuel->currentLevel == 0.0)
        return false;
    
    float newFuelLevel = fuel->currentLevel - extraFuel;
    if (newFuelLevel < 0.0) {
        fuel->currentLevel = 0.0;
        fuel->stateBar.states[0] = 0.0;
        return false;
    } else {
        fuel->currentLevel = newFuelLevel;
        fuel->stateBar.states[0] = newFuelLevel;
        return true;
    }
}

bool IWFuelExtendMaxLevel(IWFuel *fuel, float extraMaxLevel)
{
    if (fuel->currentMaxLevel == fuel->maxLevel)
        return false;
    
    float newMaxLevel = fuel->currentMaxLevel + extraMaxLevel;
    if (extraMaxLevel > fuel->maxLevel) {
        fuel->currentMaxLevel = fuel->maxLevel;
        fuel->stateBar.states[1] = 1.0;
        return false;
    } else {
        fuel->currentMaxLevel = newMaxLevel;
        fuel->stateBar.states[0] = newMaxLevel / fuel->maxLevel;
        return true;
    }
}

bool IWFuelUpdateColor(IWFuel *fuel, IWVector4 newColor, IWFUEL_COLOR fuelColor, bool updateTriangle)
{
    switch (fuelColor) {
        case IWFUEL_COLOR_CURRENT:
            fuel->stateBar.colors[0] = newColor;
            break;
        case IWFUEL_COLOR_CURRENTMAX:
            fuel->stateBar.colors[1] = newColor;
            break;
        case IWFUEL_COLOR_MAX:
            fuel->stateBar.colors[2] = newColor;
            break;
    }

    if (updateTriangle)
        IWUIStateBarToTriangles(&fuel->stateBar);

    return true;
}

size_t IWFuelToTriangleBuffer(IWFuel *fuel, GLfloat *p)
{
    fuel->stateBar.uiElementData.lineBufferStart = p;
    return IWUIStateBarToTriangles(&fuel->stateBar);
}

//size_t IWFuelToLineBuffer(IWFuel *fuel, GLfloat* p);
