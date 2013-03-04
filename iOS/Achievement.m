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
@dynamic percentComplete;
@dynamic completionReported;
@dynamic reportStatus;
@dynamic completionDate;
@dynamic completed;

- (void) awakeFromInsert {
    // Set default values
    self.gameCenterID = @"";
    self.percentComplete = @0.0;
    self.completionReported = @NO;
    self.reportStatus = @NO;
    self.completionDate = [NSDate date];
    self.completed = @NO;
}

@end
