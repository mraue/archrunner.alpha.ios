//
//  IWGameOptions.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 4/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWGameOptions_h
#define ArchRunnerAlpha_IWGameOptions_h

#include <stdbool.h>

typedef struct {
    bool playMusic;
    bool playFX;
    bool invertXAxisControls;
    bool invertYAxisControls;
} IWGameOptionsData;

IWGameOptionsData IWGameOptionsMakeDefault();

#endif
