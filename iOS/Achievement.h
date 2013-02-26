//
//  Achievement.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/26/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>


@interface Achievement : NSManagedObject

@property (nonatomic, retain) NSString * gameCenterID;
@property (nonatomic, retain) NSNumber * percentageComplete;
@property (nonatomic, retain) NSNumber * completionReported;
@property (nonatomic, retain) NSNumber * reportStatus;
@property (nonatomic, retain) NSDate * completionDate;
@property (nonatomic, retain) NSNumber * completed;

@end
