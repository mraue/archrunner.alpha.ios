//
//  ge_cubes.h
//  gameone
//
//  Created by Martin Raue on 1/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef gameone_ge_cubes_h
#define gameone_ge_cubes_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

typedef struct ge_pos {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} gePos;

typedef struct ge_cube {
    gePos p;
} geCube;

geCube* ge_create_uniform_cubes(int nx, int ny, int nz, float l, float d);
void ge_print_cubes(geCube* cubes, int n);
void ge_print_pos(gePos* p, int n);
gePos* ge_cubes_to_trianglestrip(geCube* cubes, int ncubes, float l);
void ge_cube_to_trianglestrip(geCube* cube, gePos* pos, float l, int doubleFirst, int doubleLast, GLuint* nvert);
void ge_cube_to_triangles(geCube* cube, gePos* pos, float l, GLuint* nvert);

#endif
