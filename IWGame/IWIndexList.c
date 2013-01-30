//
//  IWIndexList.c
//  theBr1dge
//
//  Created by Martin Raue on 1/30/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWIndexList.h"

#include <stdio.h>
#include <stdlib.h>

#include "IWMathUtils.h"

IWIndexListData IWIndexListMake(unsigned int nEntries)
{
    IWIndexListData indexList = {
        nEntries,
        malloc(nEntries * sizeof(unsigned int)),
        malloc(nEntries * sizeof(unsigned int))
    };
    return indexList;
}

bool IWIndexListReplaceWithLast(IWIndexListData *indexList, unsigned int entry)
{
    if (entry >= indexList->nEntries) {
        return false;
    } else if (entry == indexList->nEntries - 1) {
        indexList->nEntries--;
    } else {
        indexList->nEntries--;
        indexList->map[entry] = indexList->map[indexList->nEntries];
        indexList->reverseMap[indexList->map[entry]] = entry;
    }
    return true;
}

unsigned int IWIndexListRemoveRandom(IWIndexListData *indexList)
{
    unsigned int i = (int)(IW_RAND_UNIFORM(0.0, indexList->nEntries) - 1E-4);
    if (i > indexList->nEntries - 1) {
        printf("ERROR IWIndexListRemoveRandom - index out of range\n");
        i = indexList->nEntries - 1;
    }
    unsigned int ii = indexList->map[i];
    if (IWIndexListReplaceWithLast(indexList, i)) {
        return ii;
    } else {
        return 0;
    }
}