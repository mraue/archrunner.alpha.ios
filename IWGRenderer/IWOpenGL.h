//
//  IWOpenGL.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 3/28/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWOpenGL_h
#define ArchRunnerAlpha_IWOpenGL_h

//#define IW_ANDROID

#ifdef ANDROID

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#else

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#define IW_USE_GLVAO

#endif

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#endif
