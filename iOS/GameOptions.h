//
//  GameOptions.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 4/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreData/CoreData.h>

#import "IWGameOptions.h"


@interface GameOptions : NSManagedObject

@property (nonatomic, retain) NSNumber * playMusic;
@property (nonatomic, retain) NSNumber * playFX;
@property (nonatomic, retain) NSNumber * invertXAxisControls;
@property (nonatomic, retain) NSNumber * invertYAxisControls;

- (void)setToGameOptions:(IWGameOptionsData*)gameOptions;
- (void)setFromGameOptions:(IWGameOptionsData*)gameOptions;

@end
