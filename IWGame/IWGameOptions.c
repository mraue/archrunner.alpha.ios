//
//  IWGameOptions.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 4/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGameOptions.h"

#include <stdio.h>

IWGameOptionsData IWGameOptionsMakeDefault()
{
    IWGameOptionsData gameOptions = {
        true, true, false, false
    };
    return gameOptions;
}
