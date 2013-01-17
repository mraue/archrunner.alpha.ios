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
#import "IWUIHeadUpDisplay.h"
#import "IWUIRectangleButton.h"
#import "IWGeometry.h"

#import "IWGShaderProgram.h"
#import "ge_cubes.h"


#import "IWGameData.h"
#import "IWGame.h"
#import "IWGRenderer.h"

#import "IWFileTools.h"

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
    
    IWControllerData controllerDataAccelerometer;
    IWControllerData controllerDataCompass;

    BOOL orientationNeutralSetAccelerometer;
    BOOL orientationNeutralSetCompass;
    BOOL switchColor;

    CLLocationManager *locationManager;
    CMMotionManager *motionManager;
    
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
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.preferredFramesPerSecond = 30;

    switchColor = NO;
    
    [aButton addTarget:self action:@selector(updateControllerNeutralPosition) forControlEvents:(UIControlEventTouchDown)];
    switch1.on = NO;
    
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
    CMAccelerometerData *newestAccel = motionManager.accelerometerData;
    _filteredAcceleration.x = newestAccel.acceleration.x;
    _filteredAcceleration.y = newestAccel.acceleration.y;
    _filteredAcceleration.z = newestAccel.acceleration.z;
    
    IWControllerData controllerDataTmp = {
        {0.0, 0.0, 0.0},// referenceDirection
        {0.0, 0.0, 0.0},// direction
        {2.5, 1.0, 0.0},// pitchAngleMin
        {8.0, 4.2, 1.0},// pitchAngleMax
        {0.0, 0.0, 0.0}// rotationSpeed
    };
    controllerDataAccelerometer = controllerDataTmp;
    IWControllerData controllerDataTmp2 = {
        {0.0, 0.0, 0.0},// referenceDirection
        {0.0, 0.0, 0.0},// direction
        {2.0, 0.8, 0.0},// pitchAngleMin
        {6.0, 4.0, 1.0},// pitchAngleMax
        {0.0, 0.0, 0.0}// rotationSpeed
    };
    controllerDataCompass = controllerDataTmp;
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

//    IWGRendererSetupGL([vertShaderPathname UTF8String],
//                       [fragShaderPathname UTF8String],
//                       self.view.bounds.size.width,
//                       self.view.bounds.size.height);
    IWGRendererSetupGL([[NSString stringWithContentsOfFile:vertShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String],
                       [[NSString stringWithContentsOfFile:fragShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String],
                       self.view.bounds.size.width,
                       self.view.bounds.size.height);
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
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
    float alpha = 0.2;
    CMAccelerometerData *newestAccel = motionManager.accelerometerData;
    _filteredAcceleration.x = _filteredAcceleration.x * (1.0-alpha) + newestAccel.acceleration.x * alpha;
    _filteredAcceleration.y = _filteredAcceleration.y * (1.0-alpha) + newestAccel.acceleration.y * alpha;
    _filteredAcceleration.z = _filteredAcceleration.z * (1.0-alpha) + newestAccel.acceleration.z * alpha;
    
    if (!orientationNeutralSetAccelerometer) {
        controllerDataAccelerometer.referenceDirection = IWVector3Normalize(IWVector3Make(_filteredAcceleration.x, _filteredAcceleration.y, _filteredAcceleration.z));
        orientationNeutralSetAccelerometer = YES;
    }
    controllerDataAccelerometer.direction = IWVector3Normalize(IWVector3Make(_filteredAcceleration.x, _filteredAcceleration.y, _filteredAcceleration.z));
    
    IWControllerUpdateRotationSpeed(&controllerDataAccelerometer, self.timeSinceLastUpdate);

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
    GLKMatrix4 projectionMatrix = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(65.0f), aspect, 0.01f, 100.0f);
    
    //GLKMatrix4 baseModelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);

    // Update player position
    //float speed = 110.0;
    float speed = 0.0;
    gdPlayerData.position = IWVector3Add(gdPlayerData.position,
                                         IWVector3MultiplyScalar(gdPlayerData.direction,
                                                                 self.timeSinceLastUpdate / 1000.0 * speed));
    float rotationSpeedMax = 100.0 / 180.0 * M_PI * self.timeSinceLastUpdate;
    
    // Update y rotation
    GLKVector3 dirGLV = GLKVector3Make(gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z);
    GLKVector3 upGLV = GLKVector3Make(gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    
    float rotationSpeedX = 0.0, rotationSpeedY = 0.0;
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
        }
    }
    GLKMatrix4 yRotationUpdateMatrix = GLKMatrix4MakeRotation(rotationSpeedY * rotationSpeedMax,
                                                              upGLV.x, upGLV.y, upGLV.z);
    GLKVector3 normGLV = GLKVector3CrossProduct(dirGLV, upGLV);
    GLKMatrix4 xRotationUpdateMatrix = GLKMatrix4MakeRotation(rotationSpeedX * rotationSpeedMax,
                                                              normGLV.x, normGLV.y, normGLV.z);
    GLKMatrix4 rotationUpdateMatrix = GLKMatrix4Multiply(xRotationUpdateMatrix, yRotationUpdateMatrix);
    gdPlayerData.direction = IWVector3MakeWithArray(GLKMatrix4MultiplyVector3(rotationUpdateMatrix, dirGLV).v);
    gdPlayerData.up = IWVector3MakeWithArray(GLKMatrix4MultiplyVector3(rotationUpdateMatrix, upGLV).v);
    
    //_rotationX += -1.0 * controllerData.rotationSpeed.x * rotationSpeedMax;
    //_rotationY += -1.0 * controllerData.rotationSpeed.y * rotationSpeedMax;
    
    // Display player data
    [xLabel setText:[NSString stringWithFormat:@"%.4f", gdPlayerData.position.x]];
    [yLabel setText:[NSString stringWithFormat:@"%.4f", gdPlayerData.position.y]];
    [zLabel setText:[NSString stringWithFormat:@"%.4f", gdPlayerData.position.z]];
    [rxLabel setText:[NSString stringWithFormat:@"%.3f", controllerDataAccelerometer.rotationSpeed.x]];
    [ryLabel setText:[NSString stringWithFormat:@"%.3f", controllerDataAccelerometer.rotationSpeed.y]];
    [rzLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataAccelerometer.rotationSpeed.y]];
    
//    // DEBUG
//    [xLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.x]];
//    [yLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.y]];
//    [zLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.z]];
//    [rxLabel setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeedMax.x]];
//    [ryLabel setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeedMax.y]];
//    [rzLabel setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeedMax.z]];
//    // END DEBUG
    
    // Compute the model view matrix for the object rendered with ES2
    GLKMatrix4 modelMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);
    //modelMatrix = GLKMatrix4Rotate(modelMatrix, M_PI / 4., 0.0f, 1.0f, 0.0f);
    //GLKMatrix4 viewMatrix = GLKMatrix4MakeTranslation(gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z);
//    GLKMatrix4 viewMatrix = GLKMatrix4MakeLookAt(gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z,
//                                                 gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z,
//                                                 gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
//    GLKMatrix4 viewMatrix = GLKMatrix4MakeLookAt(gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z,
//                                                 gdPlayerData.direction.x, gdPlayerData.direction.y, gdPlayerData.direction.z,
//                                                 gdPlayerData.up.x, gdPlayerData.up.y, playerData.up.z);
    
//    // This works!
//    GLKMatrix4 viewMatrixBase = GLKMatrix4MakeTranslation(playerData.position.x,
//                                                          playerData.position.y,
//                                                          playerData.position.z);
//    GLKMatrix4 viewMatrix = GLKMatrix4MakeLookAt(0., 0., 0.,
//                                                 -playerData.direction.x, -playerData.direction.y, -playerData.direction.z,
//                                                 playerData.up.x, playerData.up.y, playerData.up.z);
//    viewMatrix = GLKMatrix4Multiply(viewMatrix, viewMatrixBase);
//    
    // Check this
    GLKMatrix4 viewMatrix = GLKMatrix4MakeLookAt(gdPlayerData.position.x, gdPlayerData.position.y, gdPlayerData.position.z,
                                                 gdPlayerData.position.x + gdPlayerData.direction.x,
                                                 gdPlayerData.position.y + gdPlayerData.direction.y,
                                                 gdPlayerData.position.z + gdPlayerData.direction.z,
                                                 gdPlayerData.up.x, gdPlayerData.up.y, gdPlayerData.up.z);
    
    
    //GLKMatrix4 modelViewMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);
    //modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, M_PI / 4., 1.0f, 1.0f, 0.0f);
    //modelViewMatrix = GLKMatrix4Rotate(modelViewMatrix, _rotation, 0.0f, 1.0f, 0.0f);
//    float rotationSpeedMax = 40.0 * 180.0 / M_PI * self.timeSinceLastUpdate / 1000.0;
//    _rotationX += -1.0 * controllerData.rotationSpeed.x * rotationSpeedMax;
//    _rotationY += -1.0 * controllerData.rotationSpeed.y * rotationSpeedMax;
    //modelViewMatrix = GLKMatrix4RotateX(modelViewMatrix, _rotationX);
    //modelViewMatrix = GLKMatrix4RotateY(modelViewMatrix, _rotationY);
    
    /*projectionMatrix = GLKMatrix4RotateX(projectionMatrix, _rotationX);
    projectionMatrix = GLKMatrix4RotateY(projectionMatrix, _rotationY);*/
    //modelViewMatrix = GLKMatrix4Multiply(baseModelViewMatrix, modelViewMatrix);
    //GLKMatrix4 modelViewMatrix = GLKMatrix4Multiply(viewMatrix, modelMatrix);

    //[aLabel3 setText:[NSString stringWithFormat:@"%.2f", _rotationX]];
    
    gdNormalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelMatrix), NULL);
    
    //_modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    gdModelMatrix = modelMatrix;
    gdProjectionMatrix = projectionMatrix;
    gdViewMatrix = viewMatrix;
    
    //_rotation += self.timeSinceLastUpdate;
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
    //NSLog(@"PONG");
//    if (switchColor) {
//        _lightSource.OuterCutoff = 1.5;
//        _lightSource.InnerCutoff = 1.0;
//        _lightSource.Exponent = 8.0;
//        switchColor = NO;
//        _materialSource.Ambient = IWVector3Make(0.0, 0.0, 0.0);
//        NSLog(@"COLOR1");
//    } else {
//        _lightSource.OuterCutoff = 0.8;
//        _lightSource.InnerCutoff = .98;
//        _lightSource.Exponent = 0.6;
//        _materialSource.Ambient = IWVector3Make(0.0, 0.0, 0.1);
//        switchColor = YES;
//        NSLog(@"COLOR2");        
//    }

    //.05, .11, 8.//OuterCutoff, InnerCutoff, Exponent
    //1.05, 1.0, 8.//OuterCutoff, InnerCutoff, Exponent
}

#pragma mark -  Track touch

- (void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch* touch = [touches anyObject];
    _touchLocation  = [touch locationInView:self.view];
    _isTouched = YES;
}

- (void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch* touch = [touches anyObject];
    _touchLocation  = [touch locationInView:self.view];
    _isTouched = NO;
}

- (void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch* touch = [touches anyObject];
    _touchLocation = [touch locationInView:self.view];
    _isTouched = YES;
}

@end
