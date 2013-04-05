//
//  GameOptions.m
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 4/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import "GameOptions.h"


@implementation GameOptions

@dynamic playMusic;
@dynamic playFX;
@dynamic invertXAxisControls;
@dynamic invertYAxisControls;


- (void) awakeFromInsert
{
    [super awakeFromInsert];
    IWGameOptionsData gameOptions = IWGameOptionsMakeDefault();
    [self setFromGameOptions:&gameOptions];
}

- (void)setToGameOptions:(IWGameOptionsData*)gameOptions
{
    gameOptions->playFX = [self.playFX boolValue];
    gameOptions->playMusic = [self.playMusic boolValue];
    gameOptions->invertXAxisControls = [self.invertXAxisControls boolValue];
    gameOptions->invertYAxisControls = [self.invertYAxisControls boolValue];
}

- (void)setFromGameOptions:(IWGameOptionsData*)gameOptions
{
    self.playFX = [NSNumber numberWithBool:gameOptions->playFX];
    self.playMusic = [NSNumber numberWithBool:gameOptions->playMusic];
    self.invertXAxisControls = [NSNumber numberWithBool:gameOptions->invertXAxisControls];
    self.invertYAxisControls = [NSNumber numberWithBool:gameOptions->invertYAxisControls];
}

@end
