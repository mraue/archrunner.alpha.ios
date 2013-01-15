//
//  IWPlayer.h
//  gameoneB
//
//  Created by Martin Raue on 1/10/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameoneB_IWPlayer_h
#define gameoneB_IWPlayer_h

#include "IWMath.h"

struct _IWPlayerData {
    IWVector3 position;
    IWVector3 direction;
    IWVector3 up;
};

typedef struct _IWPlayerData IWPlayerData;

IWPlayerData IWPlayerDataMake(IWVector3 aPosition,
                              IWVector3 aDirection,
                              IWVector3 aUp);

#endif