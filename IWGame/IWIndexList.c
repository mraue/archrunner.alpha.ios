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

IWIndexListData IWIndexListMake(unsigned int nEntriesMax)
{
    IWIndexListData indexList = {
        0,
        nEntriesMax,
        malloc(nEntriesMax * sizeof(unsigned int)),
        malloc(nEntriesMax * sizeof(unsigned int))
    };
    return indexList;
}


int IWIndexListGetObjectIdForIndex(IWIndexListData *indexList, unsigned int index)
{
    if (index > indexList->nEntries - 1) {
        printf("ERROR IWIndexListGetObjectIdForIndex: Index %u out of range %u\n", index, indexList->nEntries);
        return -1;
    } else {
        return indexList->map[index];
    }
}

int IWIndexListGetIndexForObjectId(IWIndexListData *indexList, unsigned int objectId)
{
    if (objectId > indexList->nEntriesMax - 1) {
        printf("ERROR IWIndexListGetIndexForObjectId: Object id %u out of range %u\n", objectId, indexList->nEntriesMax);
        return -1;
    } else {
        unsigned int i = indexList->reverseMap[objectId];
        if (i > indexList->nEntries - 1) {
            return -1;// Not in list
        } else {
            return i;
        }
    }
}

int IWIndexListAppendObjectId(IWIndexListData *indexList, unsigned int objectId)
{
    if (objectId > indexList->nEntriesMax - 1) {
        printf("ERROR IWIndexListAppend: objectID (%u) out of range (%u)\n",
               objectId, indexList->nEntriesMax);
        return -1;
    }
    if (indexList->nEntries < indexList->nEntriesMax) {
        indexList->map[indexList->nEntries] = objectId;
        indexList->reverseMap[objectId] = indexList->nEntries;
        indexList->nEntries++;
        return indexList->nEntries - 1;
    } else {
        printf("ERROR IWIndexListAppend: List is full\n");
        return -1;
    }
}

int IWIndexListReplaceWithLast(IWIndexListData *indexList, unsigned int entry)
{
    if (indexList->nEntries == 0) {
        printf("ERROR IWIndexListReplaceWithLast: list is empty\n");
        return -1;
    } else if (entry > indexList->nEntries - 1) {
        printf("ERROR IWIndexListReplaceWithLast: entry %u is out of range (%u)\n", entry, indexList->nEntries);
        return -1;
    } else if (entry == indexList->nEntries - 1) {
        indexList->nEntries--;
        return entry;
    } else {
        indexList->nEntries--;
        indexList->map[entry] = indexList->map[indexList->nEntries];
        indexList->reverseMap[indexList->map[entry]] = entry;
        return indexList->nEntries;
    }
}

unsigned int IWIndexListRemoveRandom(IWIndexListData *indexList)
{
    if (indexList->nEntries == 0) {
        printf("ERROR IWIndexListRemoveRandom: list is empty\n");
        return 0;
    }

    unsigned int i = (unsigned int)IW_RAND_UNIFORM(0.0, indexList->nEntries);

    if (i == indexList->nEntries)
        i--;

    unsigned int ii = indexList->map[i];

    if (IWIndexListReplaceWithLast(indexList, i) != -1) {
        return ii;
    } else {
        printf("ERROR IWIndexListRemoveRandom: error removing buffer %u\n", i);
        return 0;
    }
}