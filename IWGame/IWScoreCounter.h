//
//  IWScoreCounter.h
//  theBr1dge
//
//  Created by Martin Raue on 2/8/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWScoreCounter_h
#define theBr1dge_IWScoreCounter_h

typedef enum {
    IWGAME_DIFFICULTY_RELAXED,
    IWGAME_DIFFICULTY_FAIR,
    IWGAME_DIFFICULTY_HARD
} IWGAME_DIFFICULTY;

typedef struct {
    IWGAME_DIFFICULTY difficulty;
    float runningTimeTotal;
    unsigned int nGridCubes;
    unsigned int nBridgeCubes;
    float zMax;
    float score;
    unsigned int scoreInt;
} IWScoreCounterData;

IWScoreCounterData IWScoreCounterMakeEmpty();

float IWScoreCounterUpdateScore(IWScoreCounterData *scoreCounter);
void IWScoreCounterPrintScore(IWScoreCounterData *scoreCounter);

#endif
