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
                  IWVector4 currentColor,
                  IWVector4 currentMaxColor,
                  IWVector4 maxColor,
                  IWRectangle rectangle,
                  IWUIElementData uiElement)
{
    float states[] = {currentLevel / maxLevel, currentMaxLevel / maxLevel, 1.0};
    IWVector4 colors[] = { currentColor, currentMaxColor, maxColor};
    
    IWFuel fuel = {
        currentLevel, currentMaxLevel, maxLevel,
        IWUIStateBarMake(3, states, colors, rectangle,
                         IWUI_ORIENTATION_VERTICAL, IWUI_DIRECTION_TO_UP)
    };
    
    return fuel;
}
IWFuel IWFuelMakeDefaultStart()
{
    IWVector4 currentColor = IWUI_COLOR_LIGHT_BLUE(0.6);
    IWVector4 currentMaxColor = IWUI_COLOR_BLUE(0.4);
    IWVector4 maxColor = IWUI_COLOR_DARK_BLUE(0.4);
    IWRectangle rectangle = {{0.97, 0.25}, {0.99, 0.75}};
    IWFuel fuel = IWFuelMake(0.6, 0.6, 1.0,
                             currentColor, currentMaxColor, maxColor,
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

bool IWFuelUpdateColor(IWFuel *fuel, IWVector4 newColor, IWFUEL_COLOR fuelColor,bool updateBuffer);

size_t IWFuelToTriangleBuffer(IWFuel *fuel, GLfloat *p)
{
    fuel->stateBar.uiElementData.lineBufferStart = p;
    return IWUIStateBarToTriangles(&fuel->stateBar);
}

//size_t IWFuelToLineBuffer(IWFuel *fuel, GLfloat* p);
