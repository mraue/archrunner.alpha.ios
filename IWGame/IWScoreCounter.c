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

unsigned int IWScoreCounterGetScore(IWScoreCounterData *scoreCounter, IWSCORECOUNTER_SCORE_TYPE type)
{
    if (type == IWSCORECOUNTER_SCORE_TYPE_TIME) {
        return scoreCounter->runningTimeTotal * SCORE_TIME;
    } else if (type == IWSCORECOUNTER_SCORE_TYPE_CUBES) {
        return scoreCounter->nGridCubesConverted * SCORE_GRID_CUBE
            + scoreCounter->nBridgeCubesCollected * SCORE_BRIDGE_CUBE;
    } else if (type == IWSCORECOUNTER_SCORE_TYPE_DISTANCE) {
        return (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * SCORE_ZMAX;
    } else {
        return 0;
    }
}

float IWScoreCounterUpdateScore(IWScoreCounterData *scoreCounter)
{
    scoreCounter->score = (scoreCounter->nGridCubesConverted * SCORE_GRID_CUBE
                           + scoreCounter->nBridgeCubesCollected * SCORE_BRIDGE_CUBE
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
    printf("  nGridCubesConverted       = %u (%.2f/%.2f%%)\n",
           scoreCounter->nGridCubesConverted,
           scoreCounter->nGridCubesConverted * SCORE_GRID_CUBE,
           scoreCounter->nGridCubesConverted * SCORE_GRID_CUBE / scoreCounter->score * 100.0);
    printf("  nBridgeCubesCollected     = %u (%.2f/%.2f%%)\n",
           scoreCounter->nBridgeCubesCollected,
           scoreCounter->nBridgeCubesCollected * SCORE_BRIDGE_CUBE,
           scoreCounter->nBridgeCubesCollected * SCORE_BRIDGE_CUBE / scoreCounter->score * 100.0);
    printf("  zMax             = %.2f (%.2f/%.2f%%)\n",
           scoreCounter->zMax,
           (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * SCORE_ZMAX,
           (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * (scoreCounter->zMax / SCORE_ZMAX_NORMALIZED) * SCORE_ZMAX / scoreCounter->score * 100.0);
}