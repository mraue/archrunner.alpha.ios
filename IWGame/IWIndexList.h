//
//  IWIndexList.h
//  theBr1dge
//
//  Created by Martin Raue on 1/30/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWIndexList_h
#define theBr1dge_IWIndexList_h

#include <stdbool.h>

typedef struct {
    unsigned int nEntries;
    unsigned int *map;
    unsigned int *reverseMap;
} IWIndexListData;

IWIndexListData IWIndexListMake(unsigned int nEntries);

bool IWIndexListReplaceWithLast(IWIndexListData *indexList, unsigned int entry);
unsigned int IWIndexListRemoveRandom(IWIndexListData *indexList);

#endif
