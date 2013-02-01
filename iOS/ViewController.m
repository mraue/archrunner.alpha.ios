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

// Uniform index.
enum
{
    UNIFORM_MODEL_MATRIX,
    UNIFORM_VIEW_MATRIX,
    UNIFORM_PROJECTION_MATRIX,
    UNIFORM_NORMAL_MATRIX,
    UNIFORM_LIGHT_DIFFUSE_COLOR,
    NUM_UNIFORMS
};
GLint uniforms[NUM_UNIFORMS];

// Attribute index.
enum
{
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    NUM_ATTRIBUTES
};

GLuint N_VERT = 0;
GLuint N_VERT2 = 0;

@interface ViewController () {
    
    bool _isTouched;
    CGPoint _touchLocation;
    
    IWVector3 _filteredAcceleration;
    IWVector3 _filteredAttitude;
    
    IWControllerData controllerDataAccelerometer;
    IWControllerData controllerDataCompass;

    BOOL orientationNeutralSetAccelerometer;
    BOOL orientationNeutralSetCompass;
    BOOL switchColor;

    CLLocationManager *locationManager;
    CMMotionManager *motionManager;
    CMAttitude *savedAttitude;
    
    UILabel *xLabel, *yLabel, *zLabel, *rxLabel, *ryLabel, *rzLabel;
    UIButton *aButton;
    UISwitch *switch1;
}
@property (strong, nonatomic) EAGLContext *context;
@property (nonatomic, retain) CLLocationManager *locationManager;
@property (nonatomic, retain) CMMotionManager *motionManager;
@property (nonatomic, retain) IBOutlet UILabel *xLabel, *yLabel, *zLabel, *rxLabel, *ryLabel, *rzLabel;
@property (nonatomic, retain) IBOutlet UIButton *aButton;
@property (nonatomic, retain) IBOutlet UISwitch *switch1;

- (void)updateControllerNeutralPosition;

@end

@implementation ViewController

@synthesize locationManager;
@synthesize motionManager;
@synthesize xLabel, yLabel, zLabel, rxLabel, ryLabel, rzLabel;
@synthesize aButton;
@synthesize switch1;

- (void)dealloc
{
    IWGRendererTearDownGL();
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [_context release];
    // Stop the compass
	[locationManager stopUpdatingHeading];
    [locationManager release];
    if (motionManager.isDeviceMotionActive)
        [motionManager stopDeviceMotionUpdates];
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    switchColor = NO;
    //[aButton addTarget:self action:@selector(updateControllerNeutralPosition) forControlEvents:(UIControlEventTouchDown)];
    switch1.on = NO;
    switch1.hidden = YES;
    
    self.preferredFramesPerSecond = 30;
    
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    self.locationManager = [[[CLLocationManager alloc] init] autorelease];
	
	// check if the hardware has a compass
	if ([CLLocationManager headingAvailable] == NO) {
		// No compass is available. This application cannot function without a compass,
        // so a dialog will be displayed and no magnetic data will be measured.
        self.locationManager = nil;
//        UIAlertView *noCompassAlert = [[UIAlertView alloc] initWithTitle:@"No Compass!" message:@"This device does not have the ability to measure magnetic fields." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
//        [noCompassAlert show];
//        [noCompassAlert release];
	} else {
        // heading service configuration
        locationManager.headingFilter = kCLHeadingFilterNone;
        
        // setup delegate callbacks
        locationManager.delegate = self;
        
        // start the compass
        [locationManager startUpdatingHeading];
    }
    
    motionManager = [[CMMotionManager alloc] init]; // motionManager is an instance variable
    motionManager.accelerometerUpdateInterval = 0.01; // 100Hz
    [motionManager startAccelerometerUpdates];

    if (motionManager.isDeviceMotionAvailable) {
        motionManager.deviceMotionUpdateInterval = 0.01;
        [motionManager startDeviceMotionUpdates];
        _filteredAcceleration.x = motionManager.deviceMotion.gravity.x;
        _filteredAcceleration.y = motionManager.deviceMotion.gravity.y;
        _filteredAcceleration.z = motionManager.deviceMotion.gravity.z;
        //NSLog(@"%f %f %f",motionManager.deviceMotion.gravity.x, motionManager.deviceMotion.gravity.y, motionManager.deviceMotion.gravity.z);
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

    
//    IWControllerData controllerDataTmp = {
//        {0.0, 0.0, 0.0},// referenceDirection
//        {0.0, 0.0, 0.0},// direction
//        {1.8, 1.4, 0.0},// pitchAngleMin
//        {4.4, 4.2, 1.0},// pitchAngleMax
//        {0.0, 0.0, 0.0}// rotationSpeed
//    };
    controllerDataAccelerometer = IWControllerDataMakeDefault();
//    IWControllerData controllerDataTmp2 = {
//        {0.0, 0.0, 0.0},// referenceDirection
//        {0.0, 0.0, 0.0},// direction
//        {2.0, 0.8, 0.0},// pitchAngleMin
//        {6.0, 4.0, 1.0},// pitchAngleMax
//        {0.0, 0.0, 0.0}// rotationSpeed
//    };
    controllerDataCompass = IWControllerDataMakeDefault();
//    playerData = IWPlayerDataMake(IWVector3Make(0.0, 0.0, 0.0),
//                                  IWVector3Normalize(IWVector3Make(0.0, 0.0, 1.0)),
//                                  IWVector3Make(0.0, 1.0, 0.0));

//    playerData = IWPlayerDataMake(IWVector3Make(2.0, 2.0, 0.0),
//                                  IWVector3Normalize(IWVector3Make(-1.0, -1.0, 0.0)),
//                                  IWVector3Make(0.0, 1.0, 0.0));
    
    orientationNeutralSetAccelerometer = NO;
    orientationNeutralSetCompass = NO;
    
    IWGameSetup();
    
    [EAGLContext setCurrentContext:self.context];

    NSString *vertShaderPathname, *fragShaderPathname;
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader" ofType:@"vsh"];
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader" ofType:@"fsh"];

    IWGRendererSetupGL([vertShaderPathname UTF8String],
                       [fragShaderPathname UTF8String],
                       self.view.bounds.size.width,
                       self.view.bounds.size.height);
//    IWGRendererSetupGL([[NSString stringWithContentsOfFile:vertShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String],
//                       [[NSString stringWithContentsOfFile:fragShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String],
//                       self.view.bounds.size.width,
//                       self.view.bounds.size.height);
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

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    IWGameUpdate(self.timeSinceLastUpdate);

    float alpha = 0.2;
    if (motionManager.isDeviceMotionAvailable) {
        IWVector3 gravity = IWVector3Make(motionManager.deviceMotion.gravity.x, motionManager.deviceMotion.gravity.y, motionManager.deviceMotion.gravity.z);
        if (gdTotalRunTime < 0.5) {
            _filteredAcceleration.x = gravity.x;
            _filteredAcceleration.y = gravity.y;
            _filteredAcceleration.z = gravity.z;
            gdResetControllerPosition = true;
        } else {
            _filteredAcceleration.x = _filteredAcceleration.x * (1.0-alpha) + gravity.x * alpha;
            _filteredAcceleration.y = _filteredAcceleration.y * (1.0-alpha) + gravity.y * alpha;
            _filteredAcceleration.z = _filteredAcceleration.z * (1.0-alpha) + gravity.z * alpha;
        }
    } else {
        CMAccelerometerData *newestAccel = motionManager.accelerometerData;
        if (gdTotalRunTime < 0.5) {
            _filteredAcceleration.x = newestAccel.acceleration.x;
            _filteredAcceleration.y = newestAccel.acceleration.y;
            _filteredAcceleration.z = newestAccel.acceleration.z;
            gdResetControllerPosition = true;
        } else {
            _filteredAcceleration.x = _filteredAcceleration.x * (1.0-alpha) + newestAccel.acceleration.x * alpha;
            _filteredAcceleration.y = _filteredAcceleration.y * (1.0-alpha) + newestAccel.acceleration.y * alpha;
            _filteredAcceleration.z = _filteredAcceleration.z * (1.0-alpha) + newestAccel.acceleration.z * alpha;
        }
    }
    
    if (!orientationNeutralSetAccelerometer || gdResetControllerPosition) {
        IWControllerDataUpdateReferenceDirection(&controllerDataAccelerometer, IWVector3Normalize(IWVector3Make(_filteredAcceleration.x, _filteredAcceleration.y, _filteredAcceleration.z)), IWVector3Make(0.0, 0.0, -1.0));
//        controllerDataAccelerometer.referenceDirection = IWVector3Normalize(IWVector3Make(_filteredAcceleration.x, _filteredAcceleration.y, _filteredAcceleration.z));
        orientationNeutralSetAccelerometer = YES;
        gdResetControllerPosition = false;
        savedAttitude = [motionManager.deviceMotion.attitude retain];
    }
    controllerDataAccelerometer.direction = IWVector3Normalize(IWVector3Make(_filteredAcceleration.x, _filteredAcceleration.y, _filteredAcceleration.z));
    
    if (motionManager.isDeviceMotionAvailable) {
        alpha = 0.85;
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

    IWControllerUpdateRotationSpeed(&controllerDataCompass, self.timeSinceLastUpdate);
    
//    [aLabel1 setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeed.x]];
//    [aLabel2 setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeed.y]];
//    [aLabel3 setText:[NSString stringWithFormat:@"%.1f", controllerData.rotationSpeed.z]];
    
//    [aLabel1 setText:[NSString stringWithFormat:@"%.2f", _filteredAcceleration.x]];
//    [aLabel2 setText:[NSString stringWithFormat:@"%.2f", _filteredAcceleration.y]];
//    [aLabel3 setText:[NSString stringWithFormat:@"%.2f", _filteredAcceleration.z]];
    
    //[aLabel setText:[NSString stringWithFormat:@"%.1f", controllerData.orientationNeutral.x]];
    //[aLabel setText:[NSString stringWithFormat:@"%.1f", controllerData.orientation.x]];
    
    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    
    // REFACTOR: does not change, could only be created and intialized to uniform once
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.01f, 100.0f);
    
    //GLKMatrix4 baseModelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);

    // Update player position
    float speed = gdPlayerData.speed;
    //float speed = 0.0;
    gdPlayerData.position = IWVector3Add(gdPlayerData.position,
                                         IWVector3MultiplyScalar(IWVector3Normalize(gdPlayerData.direction),
                                                                 self.timeSinceLastUpdate * speed));
    float rotationSpeedMax = 100.0 / 180.0 * M_PI * self.timeSinceLastUpdate;
    
    // Update y rotation
    GLKVector3 dirGLV = GLKVector3Make(gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z);
    GLKVector3 upGLV = GLKVector3Make(gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    
    float rotationSpeedX = 0.0, rotationSpeedY = 0.0, rotationSpeedZ = 0.0;
    if (_isTouched) {
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
        if (switch1.on) {
            rotationSpeedX = controllerDataCompass.rotationSpeed.x;
            rotationSpeedY = controllerDataCompass.rotationSpeed.y;
        } else {
            rotationSpeedX = controllerDataAccelerometer.rotationSpeed.x;
            rotationSpeedY = controllerDataAccelerometer.rotationSpeed.y;
            rotationSpeedZ = controllerDataAccelerometer.rotationSpeed.z;
        }
    }
    GLKMatrix4 yRotationUpdateMatrix = GLKMatrix4MakeRotation(rotationSpeedY * rotationSpeedMax,
                                                              upGLV.x, upGLV.y, upGLV.z);
    GLKVector3 normGLV = GLKVector3CrossProduct(dirGLV, upGLV);
    GLKMatrix4 xRotationUpdateMatrix = GLKMatrix4MakeRotation(rotationSpeedX * rotationSpeedMax,
                                                              normGLV.x, normGLV.y, normGLV.z);
    GLKMatrix4 rotationUpdateMatrix = GLKMatrix4Multiply(xRotationUpdateMatrix, yRotationUpdateMatrix);

    if (motionManager.isDeviceMotionAvailable) {
        GLKMatrix4 zRotationUpdateMatrix = GLKMatrix4MakeRotation(rotationSpeedZ * rotationSpeedMax,
                                                                  dirGLV.x, dirGLV.y, dirGLV.z);
        rotationUpdateMatrix = GLKMatrix4Multiply(rotationUpdateMatrix, zRotationUpdateMatrix);
    }

    gdPlayerData.direction = IWVector3MakeWithArray(GLKMatrix4MultiplyVector3(rotationUpdateMatrix, dirGLV).v);
    gdPlayerData.up = IWVector3MakeWithArray(GLKMatrix4MultiplyVector3(rotationUpdateMatrix, upGLV).v);
    
    // Display player data
    [xLabel setText:[NSString stringWithFormat:@"%.4f", gdPlayerData.position.x]];
    [yLabel setText:[NSString stringWithFormat:@"%.4f", gdPlayerData.position.y]];
    [zLabel setText:[NSString stringWithFormat:@"%.4f", gdPlayerData.position.z]];
    [rxLabel setText:[NSString stringWithFormat:@"%u", gdStandardCubeIndexList.nEntries]];
    [ryLabel setText:[NSString stringWithFormat:@"%u", gdNOverdriveCubes]];
    [rzLabel setText:[NSString stringWithFormat:@"%.2f", gdZMax]];
    
    // this is how to do this correctly
    //CMAttitude *currentAttitude = motionManager.deviceMotion.attitude;
    //[currentAttitude multiplyByInverseOfAttitude: savedAttitude];
//    // DEBUG
//    [xLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.x]];
//    [yLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.y]];
//    [zLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.z]];
//    [rxLabel setText:[NSString stringWithFormat:@"%.2f", motionManager.deviceMotion.gravity.x]];
//    [ryLabel setText:[NSString stringWithFormat:@"%.2f", motionManager.deviceMotion.gravity.y]];
//    [rzLabel setText:[NSString stringWithFormat:@"%.2f", motionManager.deviceMotion.gravity.z]];
    
//    [rxLabel setText:[NSString stringWithFormat:@"%.2f", currentAttitude.roll * IW_RAD_TO_DEG]];
//    [ryLabel setText:[NSString stringWithFormat:@"%.2f", currentAttitude.pitch * IW_RAD_TO_DEG]];
//    [rzLabel setText:[NSString stringWithFormat:@"%.2f", currentAttitude.yaw * IW_RAD_TO_DEG]];
//    [rxLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataAccelerometer.debug.x]];
//    [ryLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataAccelerometer.debug.y]];
//    [rzLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataAccelerometer.debug.z]];

//    // END DEBUG
    
    // Compute the model view matrix for the object rendered with ES2
    // REFACTOR: does not change, could only be calculated and intialized to uniforms once
    GLKMatrix4 modelMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);
    
    // Check this
    GLKMatrix4 viewMatrix;
    if (!gdDropCamera) {
        viewMatrix = GLKMatrix4MakeLookAt(gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z,
                                          gdPlayerData.position.x + gdPlayerData.direction.x,
                                          gdPlayerData.position.y + gdPlayerData.direction.y,
                                          gdPlayerData.position.z + gdPlayerData.direction.z,
                                          gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    } else {
        viewMatrix = GLKMatrix4MakeLookAt(gdPlayerDataSave.position.x,
                                          gdPlayerDataSave.position.y,
                                          gdPlayerDataSave.position.z,
                                          gdPlayerData.position.x,
                                          gdPlayerData.position.y,
                                          gdPlayerData.position.z,
                                          gdPlayerDataSave.up.x, gdPlayerDataSave.up.y, gdPlayerDataSave.up.z);
    }

    gdNormalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelMatrix), NULL);
    gdModelMatrix = modelMatrix;
    gdProjectionMatrix = projectionMatrix;
    gdViewMatrix = viewMatrix;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    IWGRendererRender();
}

#pragma mark -  CLLocation Manager delegate methods

// This delegate method is invoked when the location manager has heading data.
- (void)locationManager:(CLLocationManager *)manager didUpdateHeading:(CLHeading *)heading {
    if (!orientationNeutralSetCompass) {
        controllerDataCompass.referenceDirection = IWVector3Normalize(IWVector3Make(heading.x, heading.y, heading.z));
        orientationNeutralSetCompass = YES;
    } else {
        controllerDataCompass.direction = IWVector3Normalize(IWVector3Make(heading.x, heading.y, heading.z));
    }
}

// This delegate method is invoked when the location managed encounters an error condition.
- (void)locationManager:(CLLocationManager *)manager didFailWithError:(NSError *)error {
    if ([error code] == kCLErrorDenied) {
        // This error indicates that the user has denied the application's request to use location services.
        [manager stopUpdatingHeading];
    } else if ([error code] == kCLErrorHeadingFailure) {
        // This error indicates that the heading could not be determined, most likely because of strong magnetic interference.
    }
}

- (void)updateControllerNeutralPosition {
    orientationNeutralSetAccelerometer = NO;
    orientationNeutralSetCompass = NO;
    gdPlayerData.position = IWVector3Make(0.0, 0.0, 0.0);
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
