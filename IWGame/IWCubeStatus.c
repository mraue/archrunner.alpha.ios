//
//  IWCubeStatus.c
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWCubeStatus.h"

#include <stdio.h>

IWCubeStatusData IWCubeStatusMakeEmpty()
{
    IWCubeStatusData gameStatus = { 0, 0, 0 };
    return gameStatus;
}