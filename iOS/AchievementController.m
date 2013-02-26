//
//  AchievementController.m
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/26/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import "AchievementController.h"

@interface AchievementController ()

@end

@implementation AchievementController

@synthesize achievementTracker=_achievementTracker;
@synthesize achievements=_achievements;

- (void)dealloc
{
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
        [self saveContext];
    } else if ([mutableFetchResults count] == 10) {
        _achievements = mutableFetchResults;
        [_achievements retain];
    } else {
        // Handle the error.
        NSLog(@"ERROR: Found more then one AchievementTracker in data storage");
        abort();
    }
    [mutableFetchResults release];
    [request release];
    return _achievements;
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
