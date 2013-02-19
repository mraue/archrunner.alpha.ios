//
//  IWGameStatus.h
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGameStatus_h
#define theBr1dge_IWGameStatus_h

typedef struct {
    unsigned int nGridCubes, nBridgeCubes, nPoolCubes;
} IWGameStatusData;

IWGameStatusData IWGameStatusMakeEmpty();

#endif
