//
//  IWGame.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWGame.h"

#include "IWMath.h"

#include "IWGameData.h"

void IWGameSetup(void)
{
    gdPlayerData = IWPlayerDataMake(IWVector3Make(0.0, 0.0, 0.0),
                                    IWVector3Normalize(IWVector3Make(0.0, 0.0, 1.0)),
                                    IWVector3Make(0.0, 1.0, 0.0));
    return;
}

void IWGameUpdate(float timeSinceLastUpdate)
{
    return;
}