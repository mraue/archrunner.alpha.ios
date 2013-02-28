//
//  ViewController.m
//  gameone
//
//  Created by Martin Raue on 1/3/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

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

- (void)processControllInput;
- (void)authenticateLocalPlayer;
- (void)saveContext;

@end

@implementation ViewController

@synthesize motionManager=_motionManager;
@synthesize audioPlayer=_audioPlayer;
@synthesize localPlayer=_localPlayer;
@synthesize achievementController=_achievementController;

- (void)dealloc
{
    [_achievementController release];
    [_localPlayer release];
    [_motionManager release];
    [savedAttitude release];
    [_audioPlayer release];
    [_managedObjectContext release];
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
    
    printf("View dimensions: %.1f %.1f\n",
           self.view.bounds.size.width, self.view.bounds.size.height);
    printf("Screen dimensions: %.1f %.1f\n",
           [[UIScreen mainScreen] bounds].size.width, [[UIScreen mainScreen] bounds].size.height);
    
    IWGameSetup();
    
    [EAGLContext setCurrentContext:self.context];

//    NSString *vertShaderPathname, *fragShaderPathname,*vertShaderPathname2, *fragShaderPathname2;
//    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader2" ofType:@"vsh"];
//    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader2" ofType:@"fsh"];
//    vertShaderPathname2 = [[NSBundle mainBundle] pathForResource:@"MainShader" ofType:@"vsh"];
//    fragShaderPathname2 = [[NSBundle mainBundle] pathForResource:@"MainShader" ofType:@"fsh"];
    
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
    
    // Game center test
    self.achievementController = [[AchievementController alloc] initWithManagedContext:self.managedObjectContext];
    [self authenticateLocalPlayer];
//    GKGameCenterViewController *gameCenterController = [[GKGameCenterViewController alloc] init];
//    if (gameCenterController != nil)
//    {
//        gameCenterController.gameCenterDelegate = self;
//        [self presentViewController: gameCenterController animated: YES completion:nil];
//    }
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
                                                _filteredAttitude);
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
        alpha = 0.5;
        if (IWPointInRectangle(gdTouchPoint, rollLeft)) {
            gdControllerDataAccelerometer.rotationSpeed.z =
                gdControllerDataAccelerometer.rotationSpeed.z * alpha + (1.0 - alpha) * -1.0;
        } else if (IWPointInRectangle(gdTouchPoint, rollRight)) {
            gdControllerDataAccelerometer.rotationSpeed.z =
                gdControllerDataAccelerometer.rotationSpeed.z * alpha + (1.0 - alpha) * 1.0;
        }
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
    // Report achievements
    if (self.localPlayer
        && self.localPlayer.isAuthenticated
        && gdCurrentGameStatus == IWGAME_STATUS_GAME_OVER
        && gdUpdateAchievements) {
        if (self.achievementController) {
            [self.achievementController updateAchievementsWithScoreCounter:&gdScoreCounter];
        }
        gdUpdateAchievements = false;
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
