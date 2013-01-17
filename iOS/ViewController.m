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

    GLuint _program;
    GLuint _programBasicLight;
    
    IWGLightSource _lightSource;
    IWGMaterialSource _materialSource;
    
    GLKMatrix4 _modelMatrix;
    GLKMatrix4 _viewMatrix;
    GLKMatrix4 _projectionMatrix;
    GLKMatrix3 _normalMatrix;
    float _rotationX;
    float _rotationY;
    
    bool _isTouched;
    CGPoint _touchLocation;
    
    GLuint _vertexArray;
    GLuint _vertexArray2;
    GLuint _vertexBuffer;
    IWVector3 _filteredAcceleration;
    
    IWControllerData controllerDataAccelerometer;
    IWControllerData controllerDataCompass;
    IWPlayerData playerData;
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

- (void)setupGL;
- (void)tearDownGL;
//- (void)configureAccelerometer;
- (void)updateControllerNeutralPosition;

- (BOOL)loadShaders;

@end

@implementation ViewController

@synthesize locationManager;
@synthesize motionManager;
@synthesize xLabel, yLabel, zLabel, rxLabel, ryLabel, rzLabel;
@synthesize aButton;
@synthesize switch1;

- (void)dealloc
{
    [self tearDownGL];
    
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
    
    _rotationX = 0.0;
    _rotationY = 0.0;
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
    playerData = IWPlayerDataMake(IWVector3Make(0.0, 0.0, 0.0),
                                  IWVector3Normalize(IWVector3Make(0.0, 0.0, 1.0)),
                                  IWVector3Make(0.0, 1.0, 0.0));
//    playerData = IWPlayerDataMake(IWVector3Make(2.0, 2.0, 0.0),
//                                  IWVector3Normalize(IWVector3Make(-1.0, -1.0, 0.0)),
//                                  IWVector3Make(0.0, 1.0, 0.0));
    
    orientationNeutralSetAccelerometer = NO;
    orientationNeutralSetCompass = NO;
//    typedef struct _IWControllerData {
//        IWVector3 orientationNeutral;
//        IWVector3 orientation;
//        IWVector3 pitchAngleMin;
//        IWVector3 pitchAngleMax;
//        IWVector3 rotationSpeedDeltaMax;
//        IWVector3 rotationSpeed;
//        IWVector3 rotationSpeedMax;
//    } IWControllerData;
    
    _lightSource = IWGLightingMakeBasicLight();
    _materialSource = IWGLightingMakeBasicMaterial();
    
//    IWGLightSource lightSource = {
//        {3.0, 2.5, 2.0},//Position
//        {0.5, 0.1, 2.0},//Attenuation
//        {1.0, 1.0, 1.0},//Direction
//        {1.0, 1.0, 1.0},//Colour
//        1.5, 1.0, 7.0//OuterCutoff, InnerCutoff, Exponent
//        //1.05, 1.0, 8.//OuterCutoff, InnerCutoff, Exponent
//         //0.01, 0.06, 20.0
//         //.1, 1., 20.0
//    };
//    _lightSource = lightSource;
//    IWGMaterialSource materialSource = {
//        {0.0, 0.0, 0.0},// Ambient
//        {0.4, 0.4, 1.0, 1.0},// Diffuse
//        {0.0, 0.0, 0.0},// Specular
//        60.,// Shininess
//        {0.0, 0.0},
//        {0.0, 0.0}
//    };
//    _materialSource = materialSource;
//    
    [self setupGL];
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

- (void)setupGL
{
    //gePos mypos[55296];
    int nx, ny, nz;
    float d;
    
    //nx = ny = nz = 20;
    //d = 0.05;
    
    //nx = ny = nz = 27;
    //d = 0.05;

    nx = ny = nz = 10;
    d = .05;
    
    //nx = ny = nz = 4;
    //d = .1;
    
    //nx = ny = nz = 1;
    //d = 1.5;

    int n = nx * ny * nz;
    size_t mypos_size = n * 72 * sizeof(gePos);
    printf("Allocating %d position with total size %d\n", n * 72, (int)mypos_size);
    gePos *mypos = malloc(mypos_size);
    
    geCube* mycubes = ge_create_uniform_cubes(nx, ny, nz, 1., .1);
    //ge_print_cubes(mycubes, nx * ny * nz);

    //ge_cube_to_trianglestrip(mycubes, mypos, d, 1, 1, &N_VERT);
    //printf("nVertMax = %d\n", 18 + (n - 2) * 20);
    
    for (int nc=0; nc < n; nc++) {
        //ge_cube_to_trianglestrip(&mycubes[nc + 1], &mypos[N_VERT], d, 1, 1, &N_VERT);
        ge_cube_to_triangles(&mycubes[nc], &mypos[N_VERT], d, &N_VERT);
    }
    printf("nVertMax = %d\n",N_VERT);
    
    [EAGLContext setCurrentContext:self.context];
    
    [self loadShaders];
    //glUseProgram(_program);
    glUseProgram(_programBasicLight);
    
    glEnable(GL_DEPTH_TEST);
    
    // First cube
    glGenVertexArraysOES(1, &_vertexArray);
    glBindVertexArrayOES(_vertexArray);
    
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(gCubeVertexData), gCubeVertexData, GL_STATIC_DRAW);

    //glBufferData(GL_ARRAY_BUFFER, mypos_size, mypos, GL_STATIC_DRAW);
    // DEBUG
    glBufferData(GL_ARRAY_BUFFER, mypos_size, mypos, GL_DYNAMIC_DRAW);
    // DEBUG END
    
//    GLuint positionSlot = glGetAttribLocation(_program, "position");
//    GLuint normalSlot = glGetAttribLocation(_program, "normal");
    GLuint positionSlot = glGetAttribLocation(_programBasicLight, "Vertex");
    GLuint normalSlot = glGetAttribLocation(_programBasicLight, "Normal");
    GLuint colourSlot = glGetAttribLocation(_programBasicLight, "Colour");
    
//    glEnableVertexAttribArray(positionSlot);
//    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
//    glEnableVertexAttribArray(normalSlot);
//    glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normalSlot);
    glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));

    glBindVertexArrayOES(0);
    
    //
    // Head up display data
    //
    glGenVertexArraysOES(1, &_vertexArray2);
    glBindVertexArrayOES(_vertexArray2);

    float aspect = fabsf(self.view.bounds.size.width / self.view.bounds.size.height);
    printf("aspect = %f\n", aspect);
    //IWVector4 squareButtonColour = {1.0, 1.0, 1.0, 0.3};
    // Saturated yellow
    //IWVector4 squareButtonColour = {255.0 / 255.0, 223. / 255., 94. / 255, 0.2};
    // White-ish yellow
    //IWVector4 squareButtonColour = {255.0 / 255.0, 236. / 255., 147. / 255, 0.3};
    // Light gray
    IWVector4 rectangleButtonColour = {0.6, 0.6, 0.6, 0.4};
    IWUIRectangleButton rectangleButton = IWUIRectangleButtonMake(0.4, 0.0,
                                                                IWRECTANGLE_ANCHOR_POSITION_LOWER_LEFT,
                                                                0.18, 0.18, rectangleButtonColour,
                                                                IWUIRECTANGLEBUTTON_CORNER_CUT_UPPER_LEFT,
                                                                0.035, aspect);
    
    size_t mypos_size2 = 4 * 9 * 7 * sizeof(GLfloat);
    GLfloat *mypos2 = malloc(mypos_size2);

    IWUIRectangleButtonToTriangleBuffer(&rectangleButton, mypos2);
    
    glGenBuffers(1, &_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mypos_size2, mypos2, GL_DYNAMIC_DRAW);


    glEnableVertexAttribArray(positionSlot);
    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(colourSlot);
    glVertexAttribPointer(colourSlot, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), BUFFER_OFFSET(12));
    
    glBindVertexArrayOES(0);
    
//    nx = ny = nz = 4;
//    d = .1;
//    
//    n = nx * ny * nz;
//    size_t mypos_size2 = n * 32 * sizeof(gePos);
//    printf("Allocating %d position with total size %d\n", n * 32, (int)mypos_size2);
//    gePos *mypos2 = malloc(mypos_size2);
//    
//    geCube* mycubes2 = ge_create_uniform_cubes(nx, ny, nz, 1., .1);
//    //ge_print_cubes(mycubes, nx * ny * nz);
//    
//    ge_cube_to_trianglestrip(mycubes2, mypos2, d, 1, 1, &N_VERT2);
//    //printf("nVertMax = %d\n", 18 + (n - 2) * 20);
//    
//    for (int nc=0; nc < n - 1; nc++) {
//        ge_cube_to_trianglestrip(&mycubes2[nc + 1], &mypos2[N_VERT2], d, 1, 1, &N_VERT2);
//    }
//    printf("nVertMax = %d\n",N_VERT2);
//    
//    glGenVertexArraysOES(1, &_vertexArray2);
//    glBindVertexArrayOES(_vertexArray2);
//    
//    glGenBuffers(1, &_vertexBuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
//    glBufferData(GL_ARRAY_BUFFER, mypos_size2, mypos2, GL_STATIC_DRAW);
//    
//    positionSlot = glGetAttribLocation(_program, "position");
//    normalSlot = glGetAttribLocation(_program, "normal");
//
//    glEnableVertexAttribArray(positionSlot);
//    glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(0));
//    glEnableVertexAttribArray(normalSlot);
//    glVertexAttribPointer(normalSlot, 3, GL_FLOAT, GL_FALSE, 24, BUFFER_OFFSET(12));
//    
//    glBindVertexArrayOES(0);
    
    // Slowed it down :(
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    glDeleteBuffers(1, &_vertexBuffer);
    glDeleteVertexArraysOES(1, &_vertexArray);
    
    if (_program) {
        glDeleteProgram(_program);
        _program = 0;
    }
    if (_programBasicLight) {
        glDeleteProgram(_programBasicLight);
        _programBasicLight = 0;
    }
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
    playerData.position = IWVector3Add(playerData.position,
                                       IWVector3MultiplyScalar(playerData.direction,
                                                               self.timeSinceLastUpdate / 1000.0 * speed));
    float rotationSpeedMax = 100.0 / 180.0 * M_PI * self.timeSinceLastUpdate;
    
    // Update y rotation
    GLKVector3 dirGLV = GLKVector3Make(playerData.direction.x, playerData.direction.y, playerData.direction.z);
    GLKVector3 upGLV = GLKVector3Make(playerData.up.x, playerData.up.y, playerData.up.z);
    
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
    GLKMatrix4 yRotationUpdateMatrix =GLKMatrix4MakeRotation(rotationSpeedY * rotationSpeedMax,
                                                             upGLV.x, upGLV.y, upGLV.z);
    GLKVector3 normGLV = GLKVector3CrossProduct(dirGLV, upGLV);
    GLKMatrix4 xRotationUpdateMatrix =GLKMatrix4MakeRotation(rotationSpeedX * rotationSpeedMax,
                                                             normGLV.x, normGLV.y, normGLV.z);
    GLKMatrix4 rotationUpdateMatrix = GLKMatrix4Multiply(xRotationUpdateMatrix, yRotationUpdateMatrix);
    playerData.direction = IWVector3MakeWithArray(GLKMatrix4MultiplyVector3(rotationUpdateMatrix, dirGLV).v);
    playerData.up = IWVector3MakeWithArray(GLKMatrix4MultiplyVector3(rotationUpdateMatrix, upGLV).v);
    
    //_rotationX += -1.0 * controllerData.rotationSpeed.x * rotationSpeedMax;
    //_rotationY += -1.0 * controllerData.rotationSpeed.y * rotationSpeedMax;
    
    // Display player data
//    [xLabel setText:[NSString stringWithFormat:@"%.4f", playerData.position.x]];
//    [yLabel setText:[NSString stringWithFormat:@"%.4f", playerData.position.y]];
//    [zLabel setText:[NSString stringWithFormat:@"%.4f", playerData.position.z]];
    [rxLabel setText:[NSString stringWithFormat:@"%.3f", controllerDataAccelerometer.rotationSpeed.x]];
    [ryLabel setText:[NSString stringWithFormat:@"%.3f", controllerDataAccelerometer.rotationSpeed.y]];
    [rzLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataAccelerometer.rotationSpeed.y]];
    
//    // DEBUG
    [xLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.x]];
    [yLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.y]];
    [zLabel setText:[NSString stringWithFormat:@"%.2f", controllerDataCompass.rotationSpeed.z]];
//    [rxLabel setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeedMax.x]];
//    [ryLabel setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeedMax.y]];
//    [rzLabel setText:[NSString stringWithFormat:@"%.2f", controllerData.rotationSpeedMax.z]];
//    // END DEBUG
    
    // Compute the model view matrix for the object rendered with ES2
    GLKMatrix4 modelMatrix = GLKMatrix4MakeTranslation(0.0f, 0.0f, 0.0f);
    //modelMatrix = GLKMatrix4Rotate(modelMatrix, M_PI / 4., 0.0f, 1.0f, 0.0f);
    //GLKMatrix4 viewMatrix = GLKMatrix4MakeTranslation(playerData.position.x, playerData.position.y, playerData.position.z);
//    GLKMatrix4 viewMatrix = GLKMatrix4MakeLookAt(playerData.direction.x, playerData.direction.y, playerData.direction.z,
//                                                 playerData.position.x, playerData.position.y, playerData.position.z,
//                                                 playerData.up.x, playerData.up.y, playerData.up.z);
//    GLKMatrix4 viewMatrix = GLKMatrix4MakeLookAt(playerData.position.x, playerData.position.y, playerData.position.z,
//                                                 playerData.direction.x, playerData.direction.y, playerData.direction.z,
//                                                 playerData.up.x, playerData.up.y, playerData.up.z);
    
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
    GLKMatrix4 viewMatrix = GLKMatrix4MakeLookAt(playerData.position.x, playerData.position.y, playerData.position.z,
                                                 playerData.position.x + playerData.direction.x,
                                                 playerData.position.y + playerData.direction.y,
                                                 playerData.position.z + playerData.direction.z,
                                                 playerData.up.x, playerData.up.y, playerData.up.z);
    
    
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
    
    _normalMatrix = GLKMatrix3InvertAndTranspose(GLKMatrix4GetMatrix3(modelMatrix), NULL);
    
    //_modelViewProjectionMatrix = GLKMatrix4Multiply(projectionMatrix, modelViewMatrix);
    _modelMatrix = modelMatrix;
    _projectionMatrix = projectionMatrix;
    _viewMatrix = viewMatrix;
    
    //_rotation += self.timeSinceLastUpdate;
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    //glClearColor(0.65f, 0.65f, 0.65f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Draw array 1
    glBindVertexArrayOES(_vertexArray);
    
    //glUseProgram(_program);
    //glUseProgram(_program);
    
    glUniformMatrix4fv(uniforms[UNIFORM_MODEL_MATRIX], 1, 0, _modelMatrix.m);
    glUniformMatrix4fv(uniforms[UNIFORM_VIEW_MATRIX], 1, 0, _viewMatrix.m);
    glUniformMatrix4fv(uniforms[UNIFORM_PROJECTION_MATRIX], 1, 0, _projectionMatrix.m);
    glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 0.4, 0.4, 1.0, 1.0);
    _lightSource.Position = playerData.position;
    //_lightSource.Direction = IWVector3MultiplyScalar(playerData.direction, -1.0);
    _lightSource.Direction = playerData.direction;
    IWGLightingSetUniforms(_lightSource, _materialSource);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT / 2);
    glDrawArrays(GL_TRIANGLES, 0, N_VERT / 2);
    //glDrawArrays(GL_LINE_STRIP, 0, N_VERT / 2);

    glBindVertexArrayOES(0);
    
    // Draw array 2
    
    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArrayOES(_vertexArray2);
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
                0);
    
    //glUseProgram(_program);
    
    //glUniformMatrix4fv(uniforms[UNIFORM_MODELVIEWPROJECTION_MATRIX], 1, 0, _modelViewProjectionMatrix.m);
    //glUniformMatrix3fv(uniforms[UNIFORM_NORMAL_MATRIX], 1, 0, _normalMatrix.m);
    //glUniform4f(uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR], 1.0, 0.1, 0.1, 1.0);
    
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, N_VERT2 / 2);
    glDrawArrays(GL_TRIANGLES, 0, 1*9 + 3*6);
    
    glUniform1i(IWGLightingUniformLocations[IWGLIGHTING_UNIFORM_LOC_SHADER_TYPE],
                1);
    
    glBindVertexArrayOES(0);
    glDisable(GL_BLEND);
}

#pragma mark -  OpenGL ES 2 shader compilation

- (BOOL)loadShaders
{
//    GLuint vertShader, fragShader;
    NSString *vertShaderPathname, *fragShaderPathname;

//    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"vsh"];
//    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"Shader" ofType:@"fsh"];
//   
//    _program = IWGShaderProgramBuildProgram([[NSString stringWithContentsOfFile:vertShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String],
//                                [[NSString stringWithContentsOfFile:fragShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String]);
//
//    // Get uniform locations.
//    uniforms[UNIFORM_MODEL_MATRIX] = glGetUniformLocation(_program, "ModelMatrix");
//    uniforms[UNIFORM_VIEW_MATRIX] = glGetUniformLocation(_program, "ViewMatrix");
//    uniforms[UNIFORM_PROJECTION_MATRIX] = glGetUniformLocation(_program, "ProjectionMatrix");
//    //uniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(_program, "normalMatrix");
//    //uniforms[UNIFORM_LIGHT_DIFFUSE_COLOR] = glGetUniformLocation(_program, "lightDiffuseColor");

    // Basic lighting program
    vertShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader" ofType:@"vsh"];
    fragShaderPathname = [[NSBundle mainBundle] pathForResource:@"MasterShader" ofType:@"fsh"];
    
    _programBasicLight = IWGShaderProgramBuildProgram([[NSString stringWithContentsOfFile:vertShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String],
                                [[NSString stringWithContentsOfFile:fragShaderPathname encoding:NSUTF8StringEncoding error:nil] UTF8String]);
    // Get uniform locations.
    uniforms[UNIFORM_MODEL_MATRIX] = glGetUniformLocation(_programBasicLight, "ModelMatrix");
    uniforms[UNIFORM_VIEW_MATRIX] = glGetUniformLocation(_programBasicLight, "ViewMatrix");
    uniforms[UNIFORM_PROJECTION_MATRIX] = glGetUniformLocation(_programBasicLight, "ProjectionMatrix");
    uniforms[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(_programBasicLight, "NormalMatrix");
    IWGLightingInitializeUniformLocations(_programBasicLight);
    IWGLightingSetUniforms(_lightSource, _materialSource);

    return YES;
}

#
#pragma mark -  CLLocation Manager delegate methods
#

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
    playerData.position = IWVector3Make(0.0, 0.0, 0.0);
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

#
#pragma mark -  Track touch
#

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
