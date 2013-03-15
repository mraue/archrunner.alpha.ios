//
//  ViewController.m
//  gameone
//
//  Created by Martin Raue on 1/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#import "FISound.h"

#import "ViewController.h"
#import "IWController.h"
#import "IWPlayer.h"
#import "IWGLighting.h"
#import "IWUIRectangleButton.h"
#import "IWGeometry.h"

#import "IWGShaderProgram.h"

#import "IWGameData.h"
#import "IWGame.h"
#import "IWGRenderer.h"

#import "AchievementController.h"
#import "IWSoundHandler.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

@interface ViewController () {
    
    bool _isTouched;
    CGPoint _touchLocation;
    
    IWVector3 _filteredAcceleration;
    IWVector3 _filteredAttitude;

    BOOL orientationNeutralSetAccelerometer;

    CMAttitude *savedAttitude;
}

@property (strong, nonatomic) EAGLContext *context;
@property (retain, nonatomic) CMMotionManager *motionManager;
@property (retain, nonatomic) GKLocalPlayer *localPlayer;
@property (retain, nonatomic) AchievementController *achievementController;
@property (retain, nonatomic) FISoundEngine *finchSoundEngine;
@property (retain, nonatomic) NSMutableDictionary *soundFXDictionary;

- (void)processControllInput;
- (void)authenticateLocalPlayer;
- (void)saveContext;

@end

@implementation ViewController

@synthesize motionManager=_motionManager;
@synthesize audioPlayer=_audioPlayer;
@synthesize localPlayer=_localPlayer;
@synthesize achievementController=_achievementController;
@synthesize finchSoundEngine=_finchSoundEngine;
@synthesize soundFXDictionary=_soundFXDictionary;

- (void)dealloc
{
    [_achievementController release];
    [_localPlayer release];
    [_motionManager release];
    [savedAttitude release];
    [_audioPlayer release];
    [_finchSoundEngine release];
    [_managedObjectContext release];
    [_soundFXDictionary release];
    [super dealloc];
}

- (NSUInteger)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscapeLeft;
}

- (void)viewDidLoad
{
    [super viewDidLoad];

    // DEBUG
//    printf("self.achievementController = %f\n", [self.achievementController.achievementTracker.zTravelledTotal floatValue]);
//    self.achievementController.achievementTracker.zTravelledTotal = [NSNumber numberWithFloat:[self.achievementController.achievementTracker.zTravelledTotal floatValue] + 1.0];
//    for (Achievement* achievement in self.achievementController.achievements) {
//        NSLog(@"%@", achievement.gameCenterID);
//    }
//    [self.achievementController saveContext];
    // END DEBUG

    self.localPlayer = nil;
    
    // Prevent screen diming and autolock
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    
    // Are we running on a simulator
    gdRunningInSimulator = ([[[[UIDevice currentDevice] model] uppercaseString] rangeOfString:@"SIMULATOR"].location == NSNotFound) ? false : true;
    
    // Default setting for all devices
    self.preferredFramesPerSecond = 60;
    
    // Create open gl context
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    // Setup view
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    // Initialize motion manager
    self.motionManager = [[CMMotionManager alloc] init]; // motionManager is an instance variable
    self.motionManager.accelerometerUpdateInterval = 0.01; // 100Hz
    [self.motionManager startAccelerometerUpdates];

    if (self.motionManager.isDeviceMotionAvailable) {
        // IPhone 4++
        //self.preferredFramesPerSecond = 60;
        
        self.motionManager.deviceMotionUpdateInterval = 0.01;
        
        [self.motionManager startDeviceMotionUpdates];

        _filteredAcceleration.x = self.motionManager.deviceMotion.gravity.x;
        _filteredAcceleration.y = self.motionManager.deviceMotion.gravity.y;
        _filteredAcceleration.z = self.motionManager.deviceMotion.gravity.z;

        savedAttitude = [self.motionManager.deviceMotion.attitude retain];
        _filteredAttitude = IWVector3Make(savedAttitude.roll * IW_RAD_TO_DEG,
                                          -1.0 * savedAttitude.pitch * IW_RAD_TO_DEG,
                                          -1.0 * savedAttitude.yaw * IW_RAD_TO_DEG);
    } else {
        CMAccelerometerData *newestAccel = self.motionManager.accelerometerData;
        
        _filteredAcceleration.x = newestAccel.acceleration.x;
        _filteredAcceleration.y = newestAccel.acceleration.y;
        _filteredAcceleration.z = newestAccel.acceleration.z;
    }
    
    orientationNeutralSetAccelerometer = NO;
    
    gdScreenHeight = self.view.bounds.size.height;
    gdScreenWidth = self.view.bounds.size.width;
    
//    printf("View dimensions: %.1f %.1f\n",
//           self.view.bounds.size.width, self.view.bounds.size.height);
//    printf("Screen dimensions: %.1f %.1f\n",
//           [[UIScreen mainScreen] bounds].size.width, [[UIScreen mainScreen] bounds].size.height);
    
    IWGameSetup();
    
    [EAGLContext setCurrentContext:self.context];
    
    // In game sound effects via Fitch
    self.finchSoundEngine = [FISoundEngine sharedEngine];
    self.soundFXDictionary = [NSMutableDictionary dictionary];
    gdSoundHandler = IWSoundHandlerMake(10);
    
    NSDictionary *soundFXMap
    = [NSDictionary dictionaryWithObjects:[NSArray arrayWithObjects:
                                           @"soundfx_ping_01.wav",
                                           @"soundfx_grey_cube_02.wav",
                                           @"soundfx_grey_cube_01.wav",
                                           @"soundfx_cubes_spawn_02.caf",
                                           nil]
                                  forKeys:[NSArray arrayWithObjects:
                                           [NSNumber numberWithInt:IWSOUNDHANDLER_SOUNDS_MENU_SELECTED],
                                           [NSNumber numberWithInt:IWSOUNDHANDLER_SOUNDS_GREY_CUBE_TOUCHED],
                                           [NSNumber numberWithInt:IWSOUNDHANDLER_SOUNDS_RED_CUBE_TOUCHED],
                                           [NSNumber numberWithInt:IWSOUNDHANDLER_SOUNDS_CUBES_SPAWNING],
                                           nil]];
    
    [soundFXMap enumerateKeysAndObjectsUsingBlock:^(id key, id obj, BOOL *stop) {
        NSError *error = nil;
        FISound *sound = [self.finchSoundEngine soundNamed:obj maxPolyphony:4 error:&error];
        if (!sound) {
            NSLog(@"Failed to load sound: %@", error);
        } else {
            //sound.gain = 0.9;
            [self.soundFXDictionary setObject:sound forKey:key];
        }
    }];

    
    FISound *sound = [self.soundFXDictionary objectForKey:[NSNumber numberWithInt:IWSOUNDHANDLER_SOUNDS_MENU_SELECTED]];
    if (sound) {
        sound.gain = 0.7;
    }
    
    // Background music
    NSString *soundFilePath =
    [[NSBundle mainBundle] pathForResource: @"01Vladivostok"
                                    ofType: @"mp3"];
    
    NSURL *fileURL = [[NSURL alloc] initFileURLWithPath: soundFilePath];
    
    AVAudioPlayer *newPlayer =
    [[AVAudioPlayer alloc] initWithContentsOfURL: fileURL
                                           error: nil];
    [fileURL release];
    
    self.audioPlayer = newPlayer;
    [newPlayer release];
    
    [self.audioPlayer prepareToPlay];
    [self.audioPlayer setDelegate: self];
    [self.audioPlayer setNumberOfLoops:-1];
    [self.audioPlayer play];
    
    // Open GL ES shader program
    gdMainShaderProgram
        = IWGShaderProgramMakeFromFiles([[[NSBundle mainBundle] pathForResource:@"MainShader" ofType:@"vsh"] UTF8String],
                                        [[[NSBundle mainBundle] pathForResource:@"MainShader" ofType:@"fsh"] UTF8String],
                                        IWGSHADERPROGRAM_VARYING_SLOTS_SCENE);
    gdTextShaderProgram
        = IWGShaderProgramMakeFromFiles([[[NSBundle mainBundle] pathForResource:@"TextShader" ofType:@"vsh"] UTF8String],
                                        [[[NSBundle mainBundle] pathForResource:@"TextShader" ofType:@"fsh"] UTF8String],
                                        IWGSHADERPROGRAM_VARYING_SLOTS_TEXT);
    gdSkyboxShaderProgram
        = IWGShaderProgramMakeFromFiles([[[NSBundle mainBundle] pathForResource:@"SkyboxShader" ofType:@"vsh"] UTF8String],
                                        [[[NSBundle mainBundle] pathForResource:@"SkyboxShader" ofType:@"fsh"] UTF8String],
                                        IWGSHADERPROGRAM_VARYING_SLOTS_UI);
    gdUIShaderProgram
        = IWGShaderProgramMakeFromFiles([[[NSBundle mainBundle] pathForResource:@"UIShader" ofType:@"vsh"] UTF8String],
                                        [[[NSBundle mainBundle] pathForResource:@"UIShader" ofType:@"fsh"] UTF8String],
                                        IWGSHADERPROGRAM_VARYING_SLOTS_UI);
    
    // Deal with font map textures
    NSString *fontMapBaseName = @"fontmap02";
    NSString *fontMapFilename = [[NSBundle mainBundle] pathForResource:fontMapBaseName ofType:@"txt"];
    UIImage* uiImage = [[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:fontMapBaseName ofType:@"png"]]retain];
    CGImageRef cgImage = uiImage.CGImage;
    CFDataRef dataRef = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    gdFontMapTextureData = (void*)CFDataGetBytePtr(dataRef);

    IWGRendererSetupGL([fontMapFilename UTF8String]);
    
    // Game center 
    self.achievementController = [[AchievementController alloc] initWithManagedContext:self.managedObjectContext];
    [self authenticateLocalPlayer];
    gdAchievementReportedWatchingTheSunset = false;
//    GKGameCenterViewController *gameCenterController = [[GKGameCenterViewController alloc] init];
//    if (gameCenterController != nil)
//    {
//        gameCenterController.gameCenterDelegate = self;
//        [self presentViewController: gameCenterController animated: YES completion:nil];
//    }
    // DEBUG
    //[self.achievementController resetAchievements];
    // END DEBUG
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        IWGRendererTearDownGL();
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}


- (void)processControllInput
{
    float alpha = 0.2;
    if (_motionManager.isDeviceMotionAvailable) {
        IWVector3 gravity = IWVector3Make(_motionManager.deviceMotion.gravity.x,
                                          _motionManager.deviceMotion.gravity.y,
                                          _motionManager.deviceMotion.gravity.z);
        // This is somehwat of a hack to filter out the first few data sets
        // which seem to be corrupt/wrong
        if (gdTotalRunTime < 0.5) {
            _filteredAcceleration = gravity;
            gdResetControllerPosition = true;
        } else {
            _filteredAcceleration = IWVector3Lerp(_filteredAcceleration, gravity, alpha);
        }
    } else {
        CMAccelerometerData *newestAccel = _motionManager.accelerometerData;
        if (gdTotalRunTime < 0.5) {
            _filteredAcceleration = IWVector3Make(newestAccel.acceleration.x,
                                                  newestAccel.acceleration.y,
                                                  newestAccel.acceleration.z);
            gdResetControllerPosition = true;
        } else {
            _filteredAcceleration = IWVector3Lerp(_filteredAcceleration,
                                                  IWVector3Make(newestAccel.acceleration.x,
                                                                newestAccel.acceleration.y,
                                                                newestAccel.acceleration.z),
                                                  alpha);
        }
    }
    
    if (!orientationNeutralSetAccelerometer
        || gdResetControllerPosition
        || gdCurrentGameStatus == IWGAME_STATUS_PAUSED) {
        IWControllerDataUpdateReferenceDirection(&gdControllerDataAccelerometer,
                                                 IWVector3Normalize(IWVector3Make(_filteredAcceleration.x,
                                                                                  _filteredAcceleration.y,
                                                                                  _filteredAcceleration.z)
                                                                    ),
                                                 IWVector3Make(0.0, 0.0, -1.0));
        orientationNeutralSetAccelerometer = YES;
        gdResetControllerPosition = false;
        [savedAttitude release];
        savedAttitude = [_motionManager.deviceMotion.attitude retain];
    }
    
    gdControllerDataAccelerometer.direction = IWVector3Normalize(IWVector3Make(_filteredAcceleration.x,
                                                                               _filteredAcceleration.y,
                                                                               _filteredAcceleration.z));
    
    
    if (_motionManager.isDeviceMotionAvailable) {
        // Needs to be better determined under the assumption of 60 FPS
        alpha = 0.3;
        if (savedAttitude) {
            CMAttitude *currentAttitude = [_motionManager.deviceMotion.attitude retain];
            [currentAttitude multiplyByInverseOfAttitude: savedAttitude];
            _filteredAttitude = IWVector3Lerp(_filteredAttitude,
                                             IWVector3Make(currentAttitude.roll * IW_RAD_TO_DEG,
                                                            -1.0 * currentAttitude.pitch * IW_RAD_TO_DEG,
                                                            -1.0 * currentAttitude.yaw * IW_RAD_TO_DEG),
                                              alpha);
            IWControllerAttitudeToRotationSpeed(&gdControllerDataAccelerometer,
                                                _filteredAttitude,
                                                false);
            [currentAttitude release];
        } else {
            [savedAttitude release];
            savedAttitude = [_motionManager.deviceMotion.attitude retain];
        }
    } else {
        IWControllerUpdateRotationSpeed(&gdControllerDataAccelerometer, self.timeSinceLastUpdate);
    }
    
    IWRectangle rollLeft = IWRectangleMake(0.0, 0.3, 0.3, 0.7);
    IWRectangle rollRight = IWRectangleMake(0.7, 0.3, 1.0, 0.7);
    if (gdIsTouched) {
        alpha = 0.9;
        if (IWPointInRectangle(gdTouchPoint, rollLeft)) {
            gdControllerDataAccelerometer.rotationSpeed.z =
                gdControllerDataAccelerometer.rotationSpeed.z * alpha + (1.0 - alpha) * -1.0;
        } else if (IWPointInRectangle(gdTouchPoint, rollRight)) {
            gdControllerDataAccelerometer.rotationSpeed.z =
                gdControllerDataAccelerometer.rotationSpeed.z * alpha + (1.0 - alpha) * 1.0;
        }
        if (gdControllerDataAccelerometer.rotationSpeed.z > 1.0)
            gdControllerDataAccelerometer.rotationSpeed.z = 1.0;
        if (gdControllerDataAccelerometer.rotationSpeed.z < -1.0)
            gdControllerDataAccelerometer.rotationSpeed.z = -1.0;
    } else {
        gdControllerDataAccelerometer.rotationSpeed.z = 0.0;
    }
    
    // Simulator on screen controls
    if (gdIsTouched && gdRunningInSimulator) {
        float touchActiveArea = 0.25;
        float touchFracX = gdTouchPoint.x;
        float touchFracY = gdTouchPoint.y;
        if (touchFracY < touchActiveArea) {
            gdControllerDataAccelerometer.rotationSpeed.x = -1.;
        } else if (touchFracY > 1. - touchActiveArea) {
            gdControllerDataAccelerometer.rotationSpeed.x = 1.;
        }
        if (touchFracX < touchActiveArea) {
            gdControllerDataAccelerometer.rotationSpeed.y = 1.;
        } else if (touchFracX > 1. - touchActiveArea) {
            gdControllerDataAccelerometer.rotationSpeed.y = -1.;
        }
    }
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    IWSoundHandlerEmtpyAndReset(gdSoundHandler);
    
    [self processControllInput];
    
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    
    IWGameMainHandler(self.timeSinceLastUpdate, aspect);
    
    // Report score to game center leaderboard
    if (self.localPlayer
        && self.localPlayer.isAuthenticated
        && gdPushScoreToLeaderboard) {
        GKScore *scoreReporter = [[GKScore alloc] initWithCategory:@"ArchRunnerAlpha.TotalScore.Normal"];
        scoreReporter.value = (unsigned int)gdScoreCounter.score;
        scoreReporter.context = 0;
        
        [scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
            printf("Score reported!\n");
        }];
        gdPushScoreToLeaderboard = false;
    }
    
    // Process sound
    for (int i = 0; i < gdSoundHandler->nSounds; i++) {
        FISound *sound = [self.soundFXDictionary objectForKey:[NSNumber numberWithInt:gdSoundHandler->sounds[i]]];
        if (sound) {
            [sound play];
        }
    }
    
    // Report achievements
    if (self.localPlayer
        && self.localPlayer.isAuthenticated) {
        if (gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER
            && gdUpdateAchievements) {
            if (self.achievementController) {
                [self.achievementController updateAchievementsWithScoreCounter:&gdScoreCounter];
            }
            gdUpdateAchievements = false;
        } else if (gdCurrentGameStatus == IWGAME_STATUS_START_MENU
                   && !gdAchievementReportedWatchingTheSunset
                   && gdStartMenuController
                   && gdStartMenuController->currentTransitionTime > gdStartMenuController->skyBoxController.colorTransitionTime) {
            if (self.achievementController) {
                [self.achievementController reportCompletionOfAchievementWithId:@"ArchRunnerAlpha.Achievement.WatchingTheSunset"];
                gdAchievementReportedWatchingTheSunset = true;
            }
        }
    }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    IWGRendererRender();
}

#pragma mark -  Track touch

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch* touch = [touches anyObject];
    _touchLocation  = [touch locationInView:self.view];
    _isTouched = YES;
    // Update game data
    gdIsTouched = true;
    //float x = self.interfaceOrientation == UIInterfaceOrientationLandscapeLeft ? _touchLocation.x : _touchLocation.y;
    //float y = self.interfaceOrientation == UIInterfaceOrientationLandscapeLeft ? _touchLocation.y : _touchLocation.x;
    //printf("DEBUG BEGAN: %.1f %.1f %.1f %.1f\n", _touchLocation.x, _touchLocation.y, self.view.bounds.size.width, self.view.bounds.size.height);
    gdTouchPoint.x = _touchLocation.x / self.view.bounds.size.width;
    gdTouchPoint.y = 1.0 - _touchLocation.y / self.view.bounds.size.height;
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch* touch = [touches anyObject];
    _touchLocation  = [touch locationInView:self.view];
    _isTouched = NO;
    // Update game data
    gdIsTouched = false;
    gdTouchPoint.x = _touchLocation.x / self.view.bounds.size.width;
    gdTouchPoint.y = 1.0 - _touchLocation.y / self.view.bounds.size.height;
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch* touch = [touches anyObject];
    _touchLocation = [touch locationInView:self.view];
    _isTouched = YES;
    // Update game data
    gdIsTouched = true;
    //printf("DEBUG MOVED: %.1f %.1f %.1f %.1f\n", _touchLocation.x, _touchLocation.y, self.view.bounds.size.width, self.view.bounds.size.height);
    gdTouchPoint.x = _touchLocation.x / self.view.bounds.size.width;
    gdTouchPoint.y = 1.0 - _touchLocation.y / self.view.bounds.size.height;
}

#pragma mark - GameCenter delegates

- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)authenticateLocalPlayer
{
    GKLocalPlayer *localPlayerTmp = [GKLocalPlayer localPlayer];
    localPlayerTmp.authenticateHandler = ^(UIViewController *viewController, NSError *error){
        if (viewController != nil)
        {
            [self presentViewController:viewController animated:YES completion:nil];
            //[self showAuthenticationDialogWhenReasonable: viewController];
        }
        else if (localPlayerTmp.isAuthenticated)
        {
            self.localPlayer = localPlayerTmp;
            // DEBUG
            //[self.achievementController resetAchievements];
            // DEBUG END
            //[self authenticatedPlayer: localPlayer];
        }
        else
        {
            //[self disableGameCenter];
        }
    };
}

#pragma mark - Core data

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
