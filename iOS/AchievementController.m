//
//  AchievementController.m
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/26/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import "AchievementController.h"

#import <GameKit/GameKit.h>

@interface AchievementController ()

- (void)createAchievements;
- (void)addMissingAchievementsWithArray:(NSMutableArray*)array;
- (void)updateAchievements;
- (Achievement*)getAchievementForGameCenterId:(NSString*)gameCenterId;

@end

@implementation AchievementController

@synthesize achievementTracker=_achievementTracker;
@synthesize achievements=_achievements;
@synthesize achievementIds=_achievementIds;

- (void)dealloc
{
    [_achievementIds release];
    [_achievementTracker release];
    [_achievements release];
    [_managedObjectContext release];
    [super dealloc];
}

- (AchievementController*)initWithManagedContext: (NSManagedObjectContext*)managedObjectContext
{
    self = [super init];
    if (self) {
        self.managedObjectContext = managedObjectContext;
        self.achievementIds = [NSMutableArray arrayWithObjects:
                               @"ArchRunnerAlpha.Achievement.CubeCollector",
                               @"ArchRunnerAlpha.Achievement.CubeCollectorPro",
                               nil];
    }
    return self;
}

- (AchievementTracker *)achievementTracker
{
    if (_achievementTracker != nil) {
        return _achievementTracker;
    }
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    NSEntityDescription *entity = [NSEntityDescription entityForName:@"AchievementTracker" inManagedObjectContext:self.managedObjectContext];
    [request setEntity:entity];
    
    NSError *error = nil;
    NSMutableArray *mutableFetchResults = [[self.managedObjectContext executeFetchRequest:request error:&error] mutableCopy];
    if (mutableFetchResults == nil) {
        // Handle the error.
        NSLog(@"ERROR: Could not fetch AchievementTracker from data storage");
        abort();
    }
    if ([mutableFetchResults count] == 0) {
        _achievementTracker = (AchievementTracker *)[NSEntityDescription insertNewObjectForEntityForName:@"AchievementTracker" inManagedObjectContext:self.managedObjectContext];
        [self saveContext];
    } else if ([mutableFetchResults count] == 1) {
        _achievementTracker = [[mutableFetchResults objectAtIndex:0] retain];
    } else {
        // Handle the error.
        NSLog(@"ERROR: Found more then one AchievementTracker in data storage");
        abort();
    }
    [mutableFetchResults release];
    [request release];
    return _achievementTracker;
}

- (NSMutableArray *)achievements
{
    if (_achievements != nil) {
        return _achievements;
    }
    NSFetchRequest *request = [[NSFetchRequest alloc] init];
    NSEntityDescription *entity = [NSEntityDescription entityForName:@"Achievement" inManagedObjectContext:self.managedObjectContext];
    [request setEntity:entity];
    
    NSError *error = nil;
    NSMutableArray *mutableFetchResults = [[self.managedObjectContext executeFetchRequest:request error:&error] mutableCopy];
    if (mutableFetchResults == nil) {
        // Handle the error.
        NSLog(@"ERROR: Could not fetch achievements from data storage");
        abort();
    }
    if ([mutableFetchResults count] == 0) {
        // Create achievements
        [self createAchievements];
    } else if ([mutableFetchResults count] < [self.achievementIds count]) {
        [self addMissingAchievementsWithArray:mutableFetchResults];
    } else if ([mutableFetchResults count] == [self.achievementIds count]) {
        _achievements = mutableFetchResults;
        [_achievements retain];
    } else {
        // Handle the error.
        NSLog(@"ERROR: Found to many achievements in storage");
        abort();
    }
    [mutableFetchResults release];
    [request release];
    return _achievements;
}

- (void)createAchievements
{
    _achievements = [NSMutableArray arrayWithCapacity:[self.achievementIds count]];
    for (NSString *element in self.achievementIds) {
        Achievement *newAchievement = (Achievement *)[NSEntityDescription insertNewObjectForEntityForName:@"Achievement" inManagedObjectContext:self.managedObjectContext];
        newAchievement.gameCenterID = element;
        [_achievements addObject:newAchievement];
    }
    [_achievements retain];
    [self saveContext];
}

- (void)addMissingAchievementsWithArray:(NSMutableArray*)array
{
    for (NSString *element in self.achievementIds) {
        if ([array indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
            if ([((Achievement*)(obj)).gameCenterID isEqualToString:element]) {
                *stop = YES;
                return YES;
            } else {
                return NO;
            }
        }] == NSNotFound) {
            Achievement *newAchievement = (Achievement *)[NSEntityDescription insertNewObjectForEntityForName:@"Achievement" inManagedObjectContext:self.managedObjectContext];
            newAchievement.gameCenterID = element;
            [array addObject:newAchievement];
        }
    }
    _achievements = array;
    [_achievements retain];
}

- (void)updateAchievementsWithScoreCounter:(IWScoreCounterData*)scoreCounter
{
    self.achievementTracker.nBridgeCubesTotal =
        [NSNumber numberWithInt:[self.achievementTracker.nBridgeCubesTotal intValue]
         + scoreCounter->nBridgeCubesCollected];
    
    self.achievementTracker.nGridCubesTotal =
        [NSNumber numberWithInt:[self.achievementTracker.nGridCubesTotal intValue]
         + scoreCounter->nGridCubesConverted];
    
    self.achievementTracker.playTimeTotal =
        [NSNumber numberWithInt:[self.achievementTracker.playTimeTotal intValue]
         + (int)scoreCounter->runningTimeTotal];
    
    self.achievementTracker.zTravelledTotal =
        [NSNumber numberWithFloat:[self.achievementTracker.zTravelledTotal floatValue]
         + scoreCounter->zMax];
    
    [self updateAchievements];
    
    [self saveContext];
}

- (void)updateAchievements
{
    Achievement* achievement = nil;
    NSMutableArray *achievementForReport = [[NSMutableArray alloc] initWithCapacity:[self.achievements count]];
    //@"ArchRunnerAlpha.Achievement.CubeCollector"
    achievement = [self getAchievementForGameCenterId:@"ArchRunnerAlpha.Achievement.CubeCollector"];
    
    if (achievement == nil) {
        NSLog(@"ERROR: Could not find achievement with gamecenter id ArchRunnerAlpha.Achievement.CubeCollector");
    } else if (![achievement.completed boolValue]) {
        float progress = [self.achievementTracker.nGridCubesTotal floatValue] / 1000.0;
        if (progress > 1.0) {
            progress = 1.0;
            achievement.completed = [NSNumber numberWithBool:YES];
            achievement.completionReported = @YES;
            achievement.completionDate = [NSDate date];
        }
        achievement.percentageComplete = [NSNumber numberWithFloat:progress];

        GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:achievement.gameCenterID];
        achievementGC.percentComplete = [achievement.percentageComplete floatValue] * 100.0;
        achievementGC.showsCompletionBanner = YES;
        [achievementForReport addObject:achievementGC];
    } else {

    }

    //@"ArchRunnerAlpha.Achievement.CubeCollectorPro"
    achievement = [self getAchievementForGameCenterId:@"ArchRunnerAlpha.Achievement.CubeCollectorPro"];
    if (achievement == nil) {
        NSLog(@"ERROR: Could not find achievement with gamecenter id ArchRunnerAlpha.Achievement.CubeCollector");
    } else if (![achievement.completed boolValue]) {
        float progress = [self.achievementTracker.nGridCubesTotal floatValue] / 5000.0;
        if (progress > 1.0) {
            progress = 1.0;
            achievement.completed = [NSNumber numberWithBool:YES];
            achievement.completionReported = @YES;
            achievement.completionDate = [NSDate date];
        }
        achievement.percentageComplete = [NSNumber numberWithFloat:progress];
        GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:achievement.gameCenterID];
        achievementGC.percentComplete = [achievement.percentageComplete floatValue] * 100.0;
        achievementGC.showsCompletionBanner = YES;
        [achievementForReport addObject:achievementGC];
    }
    
    if ([achievementForReport count]) {
        [GKAchievement reportAchievements: achievementForReport withCompletionHandler:^(NSError *error)
         {
             if (error != nil)
             {
                 NSLog(@"Error in reporting achievements: %@", error);
             }
             [achievementForReport release];
         }];
    } else {
        [achievementForReport release];
    }
}

- (Achievement*)getAchievementForGameCenterId:(NSString*)gameCenterId
{
    Achievement* achievement = nil;
    NSUInteger i = [self.achievements indexOfObjectPassingTest:^BOOL(id obj, NSUInteger idx, BOOL *stop) {
        if ([((Achievement*)(obj)).gameCenterID isEqualToString:gameCenterId]) {
            *stop = YES;
            return YES;
        } else {
            return NO;
        }
    }];
    if (i != NSNotFound) {
        achievement = [self.achievements objectAtIndex:i];
    }
    return achievement;
}

- (void) resetAchievements
{
    for (Achievement* entry in self.achievements) {
        entry.percentageComplete = @0.0;
        entry.completed = @NO;
        entry.completionReported = @NO;
    }
    // Clear all progress saved on Game Center.
    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError *error)
     {
         if (error != nil)
             ;// handle the error.
    }];
}

- (void)saveContext
{
    NSError *error = nil;
    NSManagedObjectContext *managedObjectContext = self.managedObjectContext;
    if (managedObjectContext != nil) {
        if ([managedObjectContext hasChanges] && ![managedObjectContext save:&error]) {
            // Replace this implementation with code to handle the error appropriately.
            // abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
            NSLog(@"Unresolved error %@, %@", error, [error userInfo]);
            abort();
        }
    }
}

@end
