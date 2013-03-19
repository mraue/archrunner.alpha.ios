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
- (void)reportAchievements:(NSMutableArray*)achievementsToReport;
- (Achievement*)getAchievementForGameCenterId:(NSString*)gameCenterId;

@end

@implementation AchievementController

@synthesize achievementTracker=_achievementTracker;
@synthesize achievements=_achievements;
@synthesize achievementIds=_achievementIds;
@synthesize lastScoreTotal=_lastScoreTotal;

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
                               @"ArchRunnerAlpha.Achievement.Club10k",
                               @"ArchRunnerAlpha.Achievement.Club14k",
                               @"ArchRunnerAlpha.Achievement.Club20k",
                               @"ArchRunnerAlpha.Achievement.WatchingTheSunset",
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
    // Silent update for new achievements, works only for adding new ones
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
    
    self.lastScoreTotal = scoreCounter->score;
    
    [self updateAchievements];
    
    [self saveContext];
}

- (void)updateAchievements
{
    Achievement* achievement = nil;
    NSMutableArray *achievementsToReport = [[NSMutableArray alloc] initWithCapacity:[self.achievements count]];
    NSMutableArray *completetedAchievements = [[NSMutableArray alloc] initWithCapacity:[self.achievements count]];
    
    //@"ArchRunnerAlpha.Achievement.CubeCollector"
    achievement = [self getAchievementForGameCenterId:@"ArchRunnerAlpha.Achievement.CubeCollector"];
    
    if (achievement == nil) {
        NSLog(@"ERROR: Could not find achievement with gamecenter id ArchRunnerAlpha.Achievement.CubeCollector");
    } else if (![achievement.completed boolValue]) {
        float progress = [self.achievementTracker.nGridCubesTotal floatValue] / 500.0 * 100.0;
        if (progress > 100.0) {
            progress = 100.0;
            achievement.completed = @YES;
            [completetedAchievements addObject:achievement];
        }
        achievement.percentComplete = [NSNumber numberWithFloat:progress];

        GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:achievement.gameCenterID];
        achievementGC.percentComplete = [achievement.percentComplete floatValue];
        achievementGC.showsCompletionBanner = YES;
        [achievementsToReport addObject:achievementGC];
    } else {

    }

    //@"ArchRunnerAlpha.Achievement.CubeCollectorPro"
    achievement = [self getAchievementForGameCenterId:@"ArchRunnerAlpha.Achievement.CubeCollectorPro"];
    if (achievement == nil) {
        NSLog(@"ERROR: Could not find achievement with gamecenter id ArchRunnerAlpha.Achievement.CubeCollector");
    } else if (![achievement.completed boolValue]) {
        float progress = [self.achievementTracker.nGridCubesTotal floatValue] / 2500.0 * 100.0;
        if (progress > 100.0) {
            progress = 100.0;
            achievement.completed = @YES;
            [completetedAchievements addObject:achievement];
        }
        achievement.percentComplete = [NSNumber numberWithFloat:progress];
        GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:achievement.gameCenterID];
        achievementGC.percentComplete = [achievement.percentComplete floatValue];
        achievementGC.showsCompletionBanner = YES;
        [achievementsToReport addObject:achievementGC];
    }
    
    // @"ArchRunnerAlpha.Achievement.Club10k"
    achievement = [self getAchievementForGameCenterId:@"ArchRunnerAlpha.Achievement.Club10k"];
    if (achievement == nil) {
        NSLog(@"ERROR: Could not find achievement with gamecenter id ArchRunnerAlpha.Achievement.Club10k");
    } else if (![achievement.completed boolValue]
               && self.lastScoreTotal >= 10000) {
        achievement.completed = @YES;
        [completetedAchievements addObject:achievement];
        achievement.percentComplete = [NSNumber numberWithFloat:100.0];
        GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:achievement.gameCenterID];
        achievementGC.percentComplete = [achievement.percentComplete floatValue];
        achievementGC.showsCompletionBanner = YES;
        [achievementsToReport addObject:achievementGC];
    }
    
    // @"ArchRunnerAlpha.Achievement.Club12k"
    achievement = [self getAchievementForGameCenterId:@"ArchRunnerAlpha.Achievement.Club14k"];
    if (achievement == nil) {
        NSLog(@"ERROR: Could not find achievement with gamecenter id ArchRunnerAlpha.Achievement.Club14k");
    } else if (![achievement.completed boolValue]
               && self.lastScoreTotal >= 14000) {
        achievement.completed = @YES;
        [completetedAchievements addObject:achievement];
        achievement.percentComplete = [NSNumber numberWithFloat:100.0];
        GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:achievement.gameCenterID];
        achievementGC.percentComplete = [achievement.percentComplete floatValue];
        achievementGC.showsCompletionBanner = YES;
        [achievementsToReport addObject:achievementGC];
    }
    
    // @"ArchRunnerAlpha.Achievement.Club14k"
    achievement = [self getAchievementForGameCenterId:@"ArchRunnerAlpha.Achievement.Club20k"];
    if (achievement == nil) {
        NSLog(@"ERROR: Could not find achievement with gamecenter id ArchRunnerAlpha.Achievement.Club20k");
    } else if (![achievement.completed boolValue]
               && self.lastScoreTotal >= 20000) {
        achievement.completed = @YES;
        [completetedAchievements addObject:achievement];
        achievement.percentComplete = [NSNumber numberWithFloat:100.0];
        GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:achievement.gameCenterID];
        achievementGC.percentComplete = [achievement.percentComplete floatValue];
        achievementGC.showsCompletionBanner = YES;
        [achievementsToReport addObject:achievementGC];
    }
    
    [self reportAchievements:achievementsToReport];
}

- (void)reportCompletionOfAchievementWithId:(NSString*)Id
{
    Achievement* a = [self getAchievementForGameCenterId:Id];
    if (a) {
        if(![a.completed boolValue]) {
            a.percentComplete = @100.0;
            a.completed = @YES;
            GKAchievement *achievementGC = [[GKAchievement alloc] initWithIdentifier:Id];
            achievementGC.percentComplete = 100.0;
            achievementGC.showsCompletionBanner = YES;
            [self reportAchievements:[[NSMutableArray arrayWithObject:achievementGC] retain]];
        }
    } else {
        NSLog(@"ERROR reportCompletionOfAchievementWithId: Achievement not found %@", Id);
    }
}

- (void)reportAchievements:(NSMutableArray*)achievementsToReport
{
    // Report achievements
    if ([achievementsToReport count]) {
        [GKAchievement reportAchievements: achievementsToReport withCompletionHandler:^(NSError *error)
         {
             if (error != nil)
             {
                 NSLog(@"Error in reporting achievements: %@", error);
             } else {
                 for (GKAchievement* achievementGC in achievementsToReport) {
                     if (achievementGC.percentComplete >= 100.0) {
                         Achievement* a = [self getAchievementForGameCenterId:achievementGC.identifier];
                         if (a) {
                             a.completionReported = @YES;
                             a.completionDate = [NSDate date];
                             a.percentComplete = @100.0;
                         } else {
                             NSLog(@"ERROR reportAchievements: Could not find achievement %@",
                                   achievementGC.identifier);
                         }
                     }
                 }
             }
             [achievementsToReport release];
         }];
    } else {
        [achievementsToReport release];
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

- (void)resetAchievements
{
    for (Achievement* entry in self.achievements) {
        entry.percentComplete = @0.0;
        entry.completed = @NO;
        entry.completionReported = @NO;
    }
    // Clear all progress saved on Game Center.
    [GKAchievement resetAchievementsWithCompletionHandler:^(NSError *error)
     {
         if (error != nil) {
             NSLog(@"ERROR: Failed to reset achievements");// handle the error.
         }
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
