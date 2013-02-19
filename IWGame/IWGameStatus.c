//
//  IWGameStatus.c
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGameStatus.h"

#include <stdio.h>

IWGameStatusData IWGameStatusMakeEmpty()
{
    IWGameStatusData gameStatus = { 0, 0, 0 };
    return gameStatus;
}