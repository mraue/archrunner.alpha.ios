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

typedef enum {
    IWSCORECOUNTER_SCORE_TYPE_TIME,
    IWSCORECOUNTER_SCORE_TYPE_CUBES,
    IWSCORECOUNTER_SCORE_TYPE_DISTANCE
} IWSCORECOUNTER_SCORE_TYPE;

typedef struct {
    IWGAME_DIFFICULTY difficulty;
    float runningTimeTotal;
    unsigned int nGridCubesConverted;
    unsigned int nBridgeCubesCollected;
    float zMax;
    float score;
    unsigned int scoreInt;
} IWScoreCounterData;

IWScoreCounterData IWScoreCounterMakeEmpty();

float IWScoreCounterUpdateScore(IWScoreCounterData *scoreCounter);
void IWScoreCounterPrintScore(IWScoreCounterData *scoreCounter);

unsigned int IWScoreCounterGetScore(IWScoreCounterData *scoreCounter, IWSCORECOUNTER_SCORE_TYPE type);

#endif
