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
                  IWVector4 warningColor)
{

    IWFuel fuel = {
        currentLevel, currentMaxLevel, maxLevel, warningLevel, false,
        currentColor, currentMaxColor, maxColor, warningColor
    };
    
    return fuel;
}

IWFuel IWFuelMakeDefaultStart()
{
    IWFuel fuel = IWFuelMake(1.0, 1.0, 1.0, 0.4,
                             //IWUI_COLOR_LIGHT_BLUE(0.6), IWUI_COLOR_BLUE(0.4), IWUI_COLOR_DARK_BLUE(0.4),
                             IWUI_COLOR_WHITE(0.8), IWUI_COLOR_WHITE(0.4), IWUI_COLOR_WHITE(0.2),
                             IWVector4Make(255.0 / 255.0, 139.0 / 255.0, 139.0 / 255.0, 0.6)
                             );
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

void IWFuelToStateBar(const IWFuel *fuel, IWUIStateBar* stateBar)
{
    if (stateBar->nStates == 3) {
        stateBar->states[0] = fuel->currentLevel / fuel->maxLevel;
        stateBar->states[1] = fuel->currentMaxLevel / fuel->maxLevel;
        stateBar->states[2] = 1.0;
    } else {
        printf("ERROR IWFuelToStateBar: State bar has wrong number of states\n");
    }
    return;
}
