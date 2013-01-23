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
                  IWUIElement uiElement)
{
    IWFuel fuel = {
        currentLevel, currentMaxLevel, maxLevel,
        currentColor, currentMaxColor, maxColor,
        rectangle,
        uiElement
    };
    return fuel;
}
IWFuel IWFuelMakeDefaultStart()
{
    IWFuel fuel = {
        0.6, 0.6, 1.0,
        {255.0 / 255.0, 236. / 255., 147. / 255, 0.6},
        {196.0 / 255.0, 208. / 255., 255.0 / 255.0, 0.4},
        {196.0 / 255.0, 208. / 255., 255.0 / 255.0, 0.2},
        {{0.97, 0.25}, {0.99, 0.75}},
        IWUIElementMakeEmpty()
    };
    return fuel;
}

bool IWFuelAddFuel(IWFuel *fuel, float extraFuel)
{
    if (fuel->currentLevel == fuel->currentMaxLevel)
        return false;
    
    float newFuelLevel = fuel->currentLevel + extraFuel;
    if (newFuelLevel > fuel->currentMaxLevel) {
        fuel->currentLevel = fuel->currentMaxLevel;
        return false;
    } else {
        fuel->currentLevel = newFuelLevel;
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
        return false;
    } else {
        fuel->currentLevel = newFuelLevel;
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
        return false;
    } else {
        fuel->currentMaxLevel = newMaxLevel;
        return true;
    }
}

size_t IWFuelToTriangleBuffer(IWFuel *fuel, GLfloat *p)
{
    GLfloat *pstart = p;
    IWVector2 llV = IWVector2AddScalar(IWVector2MultiplyScalar(fuel->rectangle.lowerLeft, 2.0), -1.0);
    IWVector2 urV = IWVector2AddScalar(IWVector2MultiplyScalar(fuel->rectangle.upperRight, 2.0), -1.0);
    float z = -0.999;
    float currentFrac = fuel->currentLevel / fuel->maxLevel;
    float currentMaxFrac = fuel->currentMaxLevel / fuel->maxLevel;
    float diffY = urV.y - llV.y;
    // - //
    *p++ = llV.x; *p++ = llV.y; *p++ = z;
    *p++ = fuel->currentColor.x; *p++ = fuel->currentColor.y; *p++ = fuel->currentColor.z; *p++ = fuel->currentColor.w;
    *p++ = urV.x; *p++ = llV.y; *p++ = z;
    *p++ = fuel->currentColor.x; *p++ = fuel->currentColor.y; *p++ = fuel->currentColor.z; *p++ = fuel->currentColor.w;
    *p++ = urV.x; *p++ = llV.y + diffY * currentFrac; *p++ = z;
    *p++ = fuel->currentColor.x; *p++ = fuel->currentColor.y; *p++ = fuel->currentColor.z; *p++ = fuel->currentColor.w;
    //
    *p++ = llV.x; *p++ = llV.y; *p++ = z;
    *p++ = fuel->currentColor.x; *p++ = fuel->currentColor.y; *p++ = fuel->currentColor.z; *p++ = fuel->currentColor.w;
    *p++ = urV.x; *p++ = llV.y + diffY * currentFrac; *p++ = z;
    *p++ = fuel->currentColor.x; *p++ = fuel->currentColor.y; *p++ = fuel->currentColor.z; *p++ = fuel->currentColor.w;
    *p++ = llV.x; *p++ = llV.y + diffY * currentFrac; *p++ = z;
    *p++ = fuel->currentColor.x; *p++ = fuel->currentColor.y; *p++ = fuel->currentColor.z; *p++ = fuel->currentColor.w;
    // - //
    *p++ = llV.x; *p++ = llV.y + diffY * currentFrac; *p++ = z;
    *p++ = fuel->currentMaxColor.x; *p++ = fuel->currentMaxColor.y; *p++ = fuel->currentMaxColor.z;
    *p++ = fuel->currentMaxColor.w;
    *p++ = urV.x; *p++ = llV.y + diffY * currentFrac; *p++ = z;
    *p++ = fuel->currentMaxColor.x; *p++ = fuel->currentMaxColor.y; *p++ = fuel->currentMaxColor.z;
    *p++ = fuel->currentMaxColor.w;
    *p++ = urV.x; *p++ = llV.y  + diffY * currentMaxFrac; *p++ = z;
    *p++ = fuel->currentMaxColor.x; *p++ = fuel->currentMaxColor.y; *p++ = fuel->currentMaxColor.z;
    *p++ = fuel->currentMaxColor.w;
    // 
    *p++ = llV.x; *p++ = llV.y + diffY * currentFrac; *p++ = z;
    *p++ = fuel->currentMaxColor.x; *p++ = fuel->currentMaxColor.y; *p++ = fuel->currentMaxColor.z;
    *p++ = fuel->currentMaxColor.w;
    *p++ = urV.x; *p++ = llV.y + diffY * currentMaxFrac; *p++ = z;
    *p++ = fuel->currentMaxColor.x; *p++ = fuel->currentMaxColor.y; *p++ = fuel->currentMaxColor.z;
    *p++ = fuel->currentMaxColor.w;
    *p++ = llV.x; *p++ = llV.y + diffY * currentMaxFrac; *p++ = z;
    *p++ = fuel->currentMaxColor.x; *p++ = fuel->currentMaxColor.y; *p++ = fuel->currentMaxColor.z;
    *p++ = fuel->currentMaxColor.w;
    // - //
    *p++ = llV.x; *p++ = llV.y + diffY * currentMaxFrac; *p++ = z;
    *p++ = fuel->maxColor.x; *p++ = fuel->maxColor.y; *p++ = fuel->maxColor.z; *p++ = fuel->maxColor.w;
    *p++ = urV.x; *p++ = llV.y + diffY * currentMaxFrac; *p++ = z;
    *p++ = fuel->maxColor.x; *p++ = fuel->maxColor.y; *p++ = fuel->maxColor.z; *p++ = fuel->maxColor.w;
    *p++ = urV.x; *p++ = urV.y; *p++ = z;
    *p++ = fuel->maxColor.x; *p++ = fuel->maxColor.y; *p++ = fuel->maxColor.z; *p++ = fuel->maxColor.w;
    //
    *p++ = llV.x; *p++ = llV.y + diffY * currentMaxFrac; *p++ = z;
    *p++ = fuel->maxColor.x; *p++ = fuel->maxColor.y; *p++ = fuel->maxColor.z; *p++ = fuel->maxColor.w;
    *p++ = urV.x; *p++ = urV.y; *p++ = z;
    *p++ = fuel->maxColor.x; *p++ = fuel->maxColor.y; *p++ = fuel->maxColor.z; *p++ = fuel->maxColor.w;
    *p++ = llV.x; *p++ = urV.y; *p++ = z;
    *p++ = fuel->maxColor.x; *p++ = fuel->maxColor.y; *p++ = fuel->maxColor.z; *p++ = fuel->maxColor.w;
    //
    return (p - pstart);
}

size_t IWFuelToLineBuffer(IWFuel *fuel, GLfloat* p);
