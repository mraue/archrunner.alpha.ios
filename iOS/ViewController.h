//
//  ViewController.h
//  gameone
//
//  Created by Martin Raue on 1/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#import <CoreMotion/CoreMotion.h>

#import <AVFoundation/AVFoundation.h>

#import <GameKit/GameKit.h>

@interface ViewController : GLKViewController <AVAudioPlayerDelegate,GKGameCenterControllerDelegate>

@property (nonatomic, retain) AVAudioPlayer *audioPlayer;

@end

