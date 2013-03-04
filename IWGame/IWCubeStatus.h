//
//  IWCubeStatus.h
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWCubeStatus_h
#define theBr1dge_IWCubeStatus_h

typedef struct {
    unsigned int nGridCubes, nBridgeCubes, nPoolCubes;
} IWCubeStatusData;

IWCubeStatusData IWCubeStatusMakeEmpty();

#endif
