//
//  IWPlayer.h
//  gameoneB
//
//  Created by Martin Raue on 1/10/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWPlayer_h
#define gameoneB_IWPlayer_h

#include <stdbool.h>

#include "IWMath.h"

struct _IWPlayerData {
    IWVector3 position;
    IWVector3 direction;
    IWVector3 up;
    float speed;
    float normalSpeed;
    bool overdrive;
    float overdriveSpeed;
    float overdriveCurrentTime;
    float overdriveDuration;
    float overdriveFadeOutDuration;
};

typedef struct _IWPlayerData IWPlayerData;

IWPlayerData IWPlayerDataMakeSimple(IWVector3 aPosition,
                                    IWVector3 aDirection,
                                    IWVector3 aUp);
IWPlayerData IWPlayerDataMake(IWVector3 aPosition,
                              IWVector3 aDirection,
                              IWVector3 aUp,
                              float normalSpeed,
                              float overdriveSpeed,
                              float overdriveDuration,
                              float overdriveFadeOutDuration);

void IWPlayerActivatOverdrive(IWPlayerData *playerData);
bool IWPlayerUpdateOverdrive(IWPlayerData *playerData, float timeSinceLastUpdate);

#endif