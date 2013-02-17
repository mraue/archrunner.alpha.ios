//
//  IWPlayer.c
//  gameoneB
//
//  Created by Martin Raue on 1/10/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWPlayer.h"

IWPlayerData IWPlayerDataMakeSimple(IWVector3 aPosition,
                                    IWVector3 aDirection,
                                    IWVector3 aUp) {
    return IWPlayerDataMake(aPosition, aDirection, aUp,
                            0.14, 0.25, 3.0, 2.0);// speed default 0.12
}

IWPlayerData IWPlayerDataMake(IWVector3 aPosition,
                              IWVector3 aDirection,
                              IWVector3 aUp,
                              float normalSpeed,
                              float overdriveSpeed,
                              float overdriveDuration,
                              float overdriveFadeOutDuration)
{
    IWPlayerData player = {
        aPosition, aDirection, aUp,
        normalSpeed, normalSpeed, false, overdriveSpeed, 0.0, overdriveDuration, overdriveFadeOutDuration
    };
    return player;
}

void IWPlayerActivatOverdrive(IWPlayerData *playerData)
{
    playerData->overdrive = true;
    playerData->speed = playerData->overdriveSpeed;
    playerData->overdriveCurrentTime = 0.0;
    return;
}

bool IWPlayerUpdateOverdrive(IWPlayerData *playerData, float timeSinceLastUpdate) {
    if (playerData->overdrive) {
        playerData->overdriveCurrentTime += timeSinceLastUpdate;
        if (playerData->overdriveCurrentTime < playerData->overdriveDuration) {
            playerData->speed = playerData->overdriveSpeed;
            return true;
        } else if (playerData->overdriveCurrentTime < playerData->overdriveDuration
                   + playerData->overdriveFadeOutDuration) {
            playerData->speed = playerData->normalSpeed + (playerData->overdriveSpeed - playerData->normalSpeed) * (1.0 - (playerData->overdriveDuration + playerData->overdriveFadeOutDuration - playerData->overdriveCurrentTime) / playerData->overdriveFadeOutDuration);
            return true;
        } else {
            playerData->speed = playerData->normalSpeed;
            playerData->overdrive = false;
            return false;
        }
    } else {
        return false;
    }
}