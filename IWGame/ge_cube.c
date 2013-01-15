//
//  ge_cube.c
//  gameone
//
//  Created by Martin Raue on 1/4/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//
#include "ge_cubes.h"

#include <stdlib.h>
#include <stdio.h>

#define MAX(a, b) (a > b ? a : b)

geCube* ge_create_uniform_cubes(int nx, int ny, int nz, float l, float d)
{
    unsigned int n = nx * ny * nz;
    geCube* cubes = malloc(n * sizeof(geCube));
    geCube* cube = cubes;
    int max = MAX(nx, MAX(ny, nz));
    float sc = 2. / (max * l  + (max + 1) * d) / l;
    float p0 = sc * (l + d);
    for (int x=0; x < nx; x++) {
        for (int y=0; y < ny; y++) {
            for (int z=0; z < nz; z++) {
                cube->p.x = x * p0  - 1. + p0 - l / 2. * sc;
                cube->p.y = y * p0  - 1. + p0 - l / 2. * sc;
                cube->p.z = z * p0  - 1. + p0 - l / 2. * sc;
                cube++;
            }
        }
    }
    return cubes;
}

void ge_print_cubes(geCube* cubes, int n)
{
    while (n > 0) {
        printf("%f %f %f\n", cubes->p.x, cubes->p.y, cubes->p.z);
        cubes++;
        n--;
    }
}

void ge_set_pos(gePos* p, float x, float y, float z)
{
    p->x = x; ;p->y = y; p->z = z;
    return;
}

void ge_print_pos(gePos* p, int n)
{
    while (n > 0) {
        printf("%f %f %f\n", p->x, p->y, p->z);
        p++; n--;
    }
}

gePos* ge_cubes_to_trianglestrip(geCube* cubes, int ncubes, float l)
{
    return NULL;
}

void ge_cube_to_trianglestrip(geCube* cube, gePos* pos, float l, int doubleFirst, int doubleLast, GLuint* nvert)
{
    gePos* pstart = pos;
    float lh = l / 2.0;
    gePos cp = cube->p;
    GLfloat x = cp.x, y = cp.y, z = cp.z;
    GLfloat* p = &(pos->x);
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    pos++;
    //ge_set_pos(pos, x - lh, y - lh, z - lh);
    //ge_set_pos(++pos, 0.0, 0.0, -1.0);
    if (doubleFirst) {
        ge_set_pos(++pos, x - lh, y - lh, z - lh);
        ge_set_pos(++pos, 0.0, 0.0, -1.0);
    }
    ge_set_pos(++pos, x + lh, y - lh, z - lh);
    ge_set_pos(++pos, 0.0, 0.0, -1.0);
    ge_set_pos(++pos, x - lh, y + lh, z - lh);
    ge_set_pos(++pos, 0.0, 0.0, -1.0);
    ge_set_pos(++pos, x + lh, y + lh, z - lh);
    ge_set_pos(++pos, 0.0, 0.0, -1.0);
    ge_set_pos(++pos, x - lh, y + lh, z + lh);
    ge_set_pos(++pos, 0.0, 1.0, 0.0);
    ge_set_pos(++pos, x + lh, y + lh, z + lh);
    ge_set_pos(++pos, 0.0, 1.0, 0.0);
    ge_set_pos(++pos, x - lh, y - lh, z + lh);
    ge_set_pos(++pos, 0.0, 0.0, 1.0);
    ge_set_pos(++pos, x + lh, y - lh, z + lh);
    ge_set_pos(++pos, 0.0, 0.0, 1.0);
    //
    ge_set_pos(++pos, x - lh, y - lh, z - lh);
    ge_set_pos(++pos, 0.0, -1.0, 0.0);
    ge_set_pos(++pos, x + lh, y - lh, z - lh);
    ge_set_pos(++pos, 0.0, -1.0, 0.0);
    //
    ge_set_pos(++pos, x + lh, y - lh, z - lh);
    ge_set_pos(++pos, 1.0, 0.0, 0.0);
    ge_set_pos(++pos, x + lh, y + lh, z - lh);
    ge_set_pos(++pos, 1.0, 0.0, 0.0);
    ge_set_pos(++pos, x + lh, y - lh, z + lh);
    ge_set_pos(++pos, 1.0, 0.0, 0.0);
    ge_set_pos(++pos, x + lh, y + lh, z + lh);
    ge_set_pos(++pos, 1.0, 0.0, 0.0);
    if (doubleLast) {
        ge_set_pos(++pos, x + lh, y + lh, z + lh);
        ge_set_pos(++pos, 1.0, 0.0, 0.0);
    }
    *nvert += pos - pstart + 1;
    return;
}

void ge_cube_to_triangles(geCube* cube, gePos* pos, float l, GLuint* nvert)
{
    float lh = l / 2.0;
    gePos cp = cube->p;
    GLfloat x = cp.x, y = cp.y, z = cp.z;
    GLfloat* p = &(pos->x);
    GLfloat* pstart = &(pos->x);
    // back 1
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    // back 2
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = -1.0;
    // top 1
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    // top 2
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 1.0; *p++ = 0.0;
    // front 1
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    // front 2
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = 0.0; *p++ = 1.0;
    // bottom 1
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    // bottom 2
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 0.0; *p++ = -1.0; *p++ = 0.0;
    // right 1
    *p++ = x + lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    // right 2
    *p++ = x + lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x + lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = 1.0; *p++ = 0.0; *p++ = 0.0;
    // left 1
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z + lh;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    // left 1
    *p++ = x - lh; *p++ = y - lh; *p++ = z + lh;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y - lh; *p++ = z - lh;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    *p++ = x - lh; *p++ = y + lh; *p++ = z - lh;
    *p++ = -1.0; *p++ = 0.0; *p++ = 0.0;
    //
    *nvert += (p - pstart) / 3;
    return;
}
