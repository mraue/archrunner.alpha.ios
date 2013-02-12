//
//  IWGame.h
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGame_h
#define theBr1dge_IWGame_h

typedef enum {
    IWGAME_STATUS_RUNNING,
    IWGAME_STATUS_PAUSED,
    IWGAME_STATUS_GAME_OVER,
    IWGAME_STATUS_START_MENU,
    IWGAME_STATUS_LOADING
} IWGAME_STATUS;

void IWGameSetup(void);

void IWGameUpdate(float timeSinceLastUpdate, float aspectRatio);

void IWGameTearDown(void);

void IWGameMainHandler(float timeSinceLastUpdate, float aspectRatio);

void IWGameStartMenuHandler(float timeSinceLastUpdate, float aspectRatio);
void IWGamePauseMenuHandler(float timeSinceLastUpdate, float aspectRatio);

#endif
