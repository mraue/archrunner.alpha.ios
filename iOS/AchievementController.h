//
//  AchievementController.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/26/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <CoreData/CoreData.h>

#import "Achievement.h"
#import "AchievementTracker.h"

@interface AchievementController : NSObject

- (AchievementController *)initWithManagedContext: (NSManagedObjectContext *)managedObjectContext;

- (void)saveContext;

@property (strong, nonatomic) NSManagedObjectContext *managedObjectContext;
@property (strong, nonatomic) AchievementTracker *achievementTracker;
@property (strong, nonatomic) NSMutableArray *achievements;

@end
