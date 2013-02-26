//
//  AchievementTracker.m
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/26/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import "AchievementTracker.h"


@implementation AchievementTracker

@dynamic playTimeTotal;
@dynamic nGridCubesTotal;
@dynamic nBridgeCubesTotal;
@dynamic zTravelledTotal;

- (void) awakeFromInsert {
    // Set default values
    self.playTimeTotal = [NSNumber numberWithInt:0];
    self.nBridgeCubesTotal = [NSNumber numberWithInt:0];
    self.nBridgeCubesTotal = [NSNumber numberWithInt:0];
    self.zTravelledTotal = [NSNumber numberWithFloat:0.0];
}

@end
