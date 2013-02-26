//
//  AchievementTracker.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/26/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface AchievementTracker : NSManagedObject

@property (nonatomic, retain) NSNumber * playTimeTotal;
@property (nonatomic, retain) NSNumber * nGridCubesTotal;
@property (nonatomic, retain) NSNumber * nBridgeCubesTotal;
@property (nonatomic, retain) NSNumber * zTravelledTotal;

@end
