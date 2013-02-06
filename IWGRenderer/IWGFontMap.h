//
//  IWGFontMap.h
//  theBr1dge
//
//  Created by Martin Raue on 2/6/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGFontMap_h
#define theBr1dge_IWGFontMap_h

#include <stdbool.h>

#include "IWGFontMapEntry.h"

typedef struct {
    IWGFontMapEntryData map[128];
    bool mapIsLoaded;
} IWGFontMapData;

IWGFontMapData IWGFontMapMakeEmpty();
IWGFontMapData IWGFontMapCreateFromFile(const char* filename);


#endif
