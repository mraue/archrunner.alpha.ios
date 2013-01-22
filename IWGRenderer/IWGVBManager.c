//
//  IWGVBManager.c
//  theBr1dge
//
//  Created by Martin Raue on 1/22/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGVBManager.h"

#include <stdio.h>

void IWGVBManagerDataAppendObjectToBuffer(IWGVBManagerData* vbManagerData, unsigned int objectID)
{
    if (vbManagerData->mapNEntries < vbManagerData->mapNEntriesMax) {
        vbManagerData->bufferToObjectMap[vbManagerData->mapNEntries] = objectID;
        vbManagerData->objectToBufferMap[objectID] = vbManagerData->mapNEntries;
        vbManagerData->mapNEntries++;
    } else {
        printf("ERROR: IWGVBManagerDataAppendObjectToBuffer\n");
        printf("       Buffer us full.\n");
    }
    return;
}

void IWGVBManagerDataReplaceObjectInBufferWithLast(IWGVBManagerData* vbManagerData, unsigned int objectID)
{
    unsigned int bufferID = vbManagerData->objectToBufferMap[objectID];
    vbManagerData->mapNEntries--;
    
    if (bufferID > vbManagerData->mapNEntries) {
        printf("ERROR: IWGVBManagerDataReplaceObjectInBufferWithLast\n");
        printf("       bufferID (%u) is out of range (%u)\n", bufferID, vbManagerData->mapNEntries);
        return;
    }
    
    if (bufferID != vbManagerData->mapNEntries) {
        vbManagerData->bufferToObjectMap[bufferID] = vbManagerData->bufferToObjectMap[vbManagerData->mapNEntries];
        vbManagerData->objectToBufferMap[vbManagerData->bufferToObjectMap[bufferID]] =bufferID;
    }
    return;
}