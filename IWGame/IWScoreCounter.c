//
//  IWScoreCounter.c
//  theBr1dge
//
//  Created by Martin Raue on 2/8/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWScoreCounter.h"

#include <stdio.h>

#define SCORE_GRID_CUBE 20.0
#define SCORE_BRIDGE_CUBE 10.0
#define SCORE_TIME 2.0
#define SCORE_ZMAX 100.0
#define SCORE_ZMAX_NORMALIZED 5.0

IWScoreCounterData IWScoreCounterMakeEmpty()
{
    IWScoreCounterData scoreCounter = {
        IWGAME_DIFFICULTY_FAIR,
        0.0, 0, 0, 0.0, 0.0, 0
    };
    return scoreCounter;
}

float IWScoreCounterUpdateScore(IWScoreCounterData *scoreCounter)
{
    scoreCounter->score = (scoreCounter->nGridCubes * SCORE_GRID_CUBE
                           + scoreCounter->nBridgeCubes * SCORE_BRIDGE_CUBE
                           + scoreCounter->runningTimeTotal * SCORE_TIME
                           + (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * SCORE_ZMAX);
    scoreCounter->scoreInt = (int) scoreCounter->score;
    return scoreCounter->score;
}

void IWScoreCounterPrintScore(IWScoreCounterData *scoreCounter)
{
    printf("Score total = %.2f\n", scoreCounter->score);
    printf("  runningTimeTotal = %.2f (%.2f/%.2f%%)\n",
           scoreCounter->runningTimeTotal,
           scoreCounter->runningTimeTotal * SCORE_TIME,
           scoreCounter->runningTimeTotal * SCORE_TIME / scoreCounter->score * 100.0);
    printf("  nGridCubes       = %u (%.2f/%.2f%%)\n",
           scoreCounter->nGridCubes,
           scoreCounter->nGridCubes * SCORE_GRID_CUBE,
           scoreCounter->nGridCubes * SCORE_GRID_CUBE / scoreCounter->score * 100.0);
    printf("  nBridgeCubes     = %u (%.2f/%.2f%%)\n",
           scoreCounter->nBridgeCubes,
           scoreCounter->nBridgeCubes * SCORE_BRIDGE_CUBE,
           scoreCounter->nBridgeCubes * SCORE_BRIDGE_CUBE / scoreCounter->score * 100.0);
    printf("  zMax             = %.2f (%.2f/%.2f%%)\n",
           scoreCounter->zMax,
           (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * SCORE_ZMAX,
           (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * SCORE_ZMAX / scoreCounter->score * 100.0);
}