//
//  Achievement.m
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/26/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import "Achievement.h"


@implementation Achievement

@dynamic gameCenterID;
@dynamic percentageComplete;
@dynamic completionReported;
@dynamic reportStatus;
@dynamic completionDate;
@dynamic completed;

- (void) awakeFromInsert {
    // Set default values
    self.gameCenterID = @"";
    self.percentageComplete = [NSNumber numberWithFloat:0.0];
    self.completionReported = [NSNumber numberWithBool:NO];
    self.reportStatus = [NSNumber numberWithBool:NO];
    self.completionDate = [NSDate date];
    self.completed = [NSNumber numberWithBool:NO];
}

@end
