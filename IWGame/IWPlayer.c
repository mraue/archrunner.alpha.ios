//
//  IWPlayer.c
//  gameoneB
//
//  Created by Martin Raue on 1/10/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWPlayer.h"

IWPlayerData IWPlayerDataMake(IWVector3 aPosition,
                              IWVector3 aDirection,
                              IWVector3 aUp) {
    IWPlayerData player = {
        aPosition,
        aDirection,
        aUp
    };
    return player;
}