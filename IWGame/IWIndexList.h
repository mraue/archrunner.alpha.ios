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
    unsigned int nEntriesMax;
    unsigned int *map;// index -> object id
    unsigned int *reverseMap;// object id -> index
} IWIndexListData;

IWIndexListData IWIndexListMake(unsigned int nEntriesMax);

int IWIndexListGetObjectIdForIndex(IWIndexListData *indexList, unsigned int index);
int IWIndexListGetIndexForObjectId(IWIndexListData *indexList, unsigned int objectId);

int IWIndexListAppendObjectId(IWIndexListData *indexList, unsigned int objectId);

int IWIndexListReplaceWithLast(IWIndexListData *indexList, unsigned int entry);
unsigned int IWIndexListRemoveRandom(IWIndexListData *indexList);

void IWIndexListDeallocData(IWIndexListData *indexList);

#endif
