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

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

@interface ViewController () {
    
    bool _isTouched;
    CGPoint _touchLocation;
    
    IWVector3 _filteredAcceleration;
    IWVector3 _filteredAttitude;
    
    IWControllerData controllerDataAccelerometer;

    BOOL orientationNeutralSetAccelerometer;

    CMMotionManager *motionManager;
    CMAttitude *savedAttitude;
}
@property (strong, nonatomic) EAGLContext *context;
@property (nonatomic, retain) CMMotionManager *motionManager;

- (void)processControllInput;

@end

@implementation ViewController

@synthesize motionManager;

- (void)dealloc
{
    IWGRendererTearDownGL();
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [_context release];
    if (motionManager.isDeviceMotionActive)
        [motionManager stopDeviceMotionUpdates];
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    // Prevent screen diming and autolock
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    
    // Are we running on a simulator
    gdRunningInSimulator = ([[[[UIDevice currentDevice] model] uppercaseString] rangeOfString:@"SIMULATOR"].location == NSNotFound) ? false : true;
    
    // Default setting for 3GS
    self.preferredFramesPerSecond = 30;
    
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
    motionManager = [[CMMotionManager alloc] init]; // motionManager is an instance variable
    motionManager.accelerometerUpdateInterval = 0.01; // 100Hz
    [motionManager startAccelerometerUpdates];

    if (motionManager.isDeviceMotionAvailable) {
        // IPhone 4++
        self.preferredFramesPerSecond = 60;
        
        motionManager.deviceMotionUpdateInterval = 0.01;
        
        [motionManager startDeviceMotionUpdates];

        _filteredAcceleration.x = motionManager.deviceMotion.gravity.x;
        _filteredAcceleration.y = motionManager.deviceMotion.gravity.y;
        _filteredAcceleration.z = motionManager.deviceMotion.gravity.z;

        savedAttitude = [motionManager.deviceMotion.attitude retain];
        _filteredAttitude = IWVector3Make(savedAttitude.roll * IW_RAD_TO_DEG,
                                          -1.0 * savedAttitude.pitch * IW_RAD_TO_DEG,
                                          -1.0 * savedAttitude.yaw * IW_RAD_TO_DEG);
    } else {
        CMAccelerometerData *newestAccel = motionManager.accelerometerData;
        
        _filteredAcceleration.x = newestAccel.acceleration.x;
        _filteredAcceleration.y = newestAccel.acceleration.y;
        _filteredAcceleration.z = newestAccel.acceleration.z;
    }

    controllerDataAccelerometer = IWControllerDataMakeDefault();
    
    orientationNeutralSetAccelerometer = NO;
    
    IWGameSetup();
    
    [EAGLContext setCurrentContext:self.context];

    NSString *vertShaderPathname, *fragShaderPathname;
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader" ofType:@"vsh"];
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader" ofType:@"fsh"];
    
    // Deal with font map textures
    NSString *fontMapFilename = [[NSBundle mainBundle] pathForResource:@"fontmap02" ofType:@"txt"];
    UIImage* uiImage = [[UIImage imageWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"fontmap02" ofType:@"png"]]retain];
    CGImageRef cgImage = uiImage.CGImage;
    CFDataRef dataRef = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    gdFontMapTextureData = (void*)CFDataGetBytePtr(dataRef);
    
    printf("View dimensions: %.1f %.1f\n",
           self.view.bounds.size.width, self.view.bounds.size.height);
    printf("Screen dimensions: %.1f %.1f\n",
           [[UIScreen mainScreen] bounds].size.width, [[UIScreen mainScreen] bounds].size.height);

    IWGRendererSetupGL([vertShaderPathname UTF8String],
                       [fragShaderPathname UTF8String],
                       [fontMapFilename UTF8String],
                       [[UIScreen mainScreen] bounds].size.width,
                       [[UIScreen mainScreen] bounds].size.height
                       );
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
    if (motionManager.isDeviceMotionAvailable) {
        IWVector3 gravity = IWVector3Make(motionManager.deviceMotion.gravity.x,
                                          motionManager.deviceMotion.gravity.y,
                                          motionManager.deviceMotion.gravity.z);
        // This is somehwat of a hack to filter out the first few data sets
        // which seem to be corrupt/wrong
        if (gdTotalRunTime < 0.5) {
            _filteredAcceleration = gravity;
            gdResetControllerPosition = true;
        } else {
            _filteredAcceleration = IWVector3Lerp(_filteredAcceleration, gravity, alpha);
        }
    } else {
        CMAccelerometerData *newestAccel = motionManager.accelerometerData;
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
        || gdGameIsPaused) {
        IWControllerDataUpdateReferenceDirection(&controllerDataAccelerometer,
                                                 IWVector3Normalize(IWVector3Make(_filteredAcceleration.x,
                                                                                  _filteredAcceleration.y,
                                                                                  _filteredAcceleration.z)
                                                                    ),
                                                 IWVector3Make(0.0, 0.0, -1.0));
        //        controllerDataAccelerometer.referenceDirection = IWVector3Normalize(IWVector3Make(_filteredAcceleration.x, _filteredAcceleration.y, _filteredAcceleration.z));
        orientationNeutralSetAccelerometer = YES;
        gdResetControllerPosition = false;
        savedAttitude = [motionManager.deviceMotion.attitude retain];
    }
    
    controllerDataAccelerometer.direction = IWVector3Normalize(IWVector3Make(_filteredAcceleration.x,
                                                                             _filteredAcceleration.y,
                                                                             _filteredAcceleration.z));
    
    
    if (motionManager.isDeviceMotionAvailable) {
        // Needs to be better determined under the assumption of 60 FPS
        alpha = 0.5;
        if (savedAttitude) {
            CMAttitude *currentAttitude = [motionManager.deviceMotion.attitude retain];
            [currentAttitude multiplyByInverseOfAttitude: savedAttitude];
            _filteredAttitude = IWVector3Add(IWVector3MultiplyScalar(_filteredAttitude, (1.0-alpha)),
                                             IWVector3MultiplyScalar(IWVector3Make(currentAttitude.roll * IW_RAD_TO_DEG,
                                                                                   -1.0 * currentAttitude.pitch * IW_RAD_TO_DEG,
                                                                                   -1.0 * currentAttitude.yaw * IW_RAD_TO_DEG),
                                                                     alpha));
            IWControllerAttitudeToRotationSpeed(&controllerDataAccelerometer,
                                                _filteredAttitude);
        } else {
            savedAttitude = [motionManager.deviceMotion.attitude retain];
        }
    } else {
        IWControllerUpdateRotationSpeed(&controllerDataAccelerometer, self.timeSinceLastUpdate);
    }
    
    // Update player position
    float speed = gdGameIsPaused ? 0.0 : gdPlayerData.speed;
    gdPlayerData.position = IWVector3Add(gdPlayerData.position,
                                         IWVector3MultiplyScalar(IWVector3Normalize(gdPlayerData.direction),
                                                                 self.timeSinceLastUpdate * speed));

    float rotationSpeedMax = 100.0 / 180.0 * M_PI * self.timeSinceLastUpdate;
    
    // Update y rotation
    IWVector3 dirGLV = IWVector3Make(gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z);
    IWVector3 upGLV = IWVector3Make(gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    
    float rotationSpeedX = 0.0, rotationSpeedY = 0.0, rotationSpeedZ = 0.0;
    
    if (_isTouched && gdRunningInSimulator) {
        float touchActiveArea = 0.25;
        float touchFracX = _touchLocation.x / self.view.bounds.size.width;
        float touchFracY = 1. - _touchLocation.y / self.view.bounds.size.height;
        if (touchFracY < touchActiveArea) {
            rotationSpeedX = -1.;
        } else if (touchFracY > 1. - touchActiveArea) {
            rotationSpeedX = 1.;
        }
        if (touchFracX < touchActiveArea) {
            rotationSpeedY = 1.;
        } else if (touchFracX > 1. - touchActiveArea) {
            rotationSpeedY = -1.;
        }
    } else {
        rotationSpeedX = controllerDataAccelerometer.rotationSpeed.x;
        rotationSpeedY = controllerDataAccelerometer.rotationSpeed.y;
        rotationSpeedZ = controllerDataAccelerometer.rotationSpeed.z;
    }
    
    IWMatrix4 yRotationUpdateMatrix = IWMatrix4MakeRotation(rotationSpeedY * rotationSpeedMax,
                                                            upGLV.x, upGLV.y, upGLV.z);
    //for (unsigned int i = 0; i < 16; i++)
    //    yRotationUpdateMatrix.m[i] = yRotationUpdateMatrix2.m[i];
    
    IWVector3 normGLV = IWVector3CrossProduct(dirGLV, upGLV);
    
    IWMatrix4 xRotationUpdateMatrix = IWMatrix4MakeRotation(rotationSpeedX * rotationSpeedMax,
                                                            normGLV.x, normGLV.y, normGLV.z);
    
    IWMatrix4 rotationUpdateMatrix = IWMatrix4Multiply(xRotationUpdateMatrix, yRotationUpdateMatrix);
    
    //    if (motionManager.isDeviceMotionAvailable) {
    //        GLKMatrix4 zRotationUpdateMatrix = GLKMatrix4MakeRotation(rotationSpeedZ * rotationSpeedMax,
    //                                                                  dirGLV.x, dirGLV.y, dirGLV.z);
    //        rotationUpdateMatrix = GLKMatrix4Multiply(rotationUpdateMatrix, zRotationUpdateMatrix);
    //    }
    
    //    if (!gdGameIsPaused) {
    gdPlayerData.direction = IWMatrix4MultiplyVector3(rotationUpdateMatrix, dirGLV);
    gdPlayerData.up = IWMatrix4MultiplyVector3(rotationUpdateMatrix, upGLV);
    //    }
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    [self processControllInput];
    
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    
    IWGameMainHandler(self.timeSinceLastUpdate, aspect);
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

@end
