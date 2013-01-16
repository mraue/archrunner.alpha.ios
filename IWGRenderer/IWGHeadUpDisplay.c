//
//  IWGHeadUpDisplay.c
//  theBr1dge
//
//  Created by Martin Raue on 1/15/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>

#include "IWGHeadUpDisplay.h"

void IWGHeadUpDisplayButtonToTriangles(IWGButton button, GLfloat *p, int *nVertices)
{
    //float* p = &vertices->x;
    float* pstart = p;
    *p++ = button.lowerLeft.x; *p++ = button.lowerLeft.y; *p++ = -1.0;
    *p++ = button.colour.x; *p++ = button.colour.y; *p++ = button.colour.z; *p++ = button.colour.w;
    *p++ = button.upperRight.x; *p++ = button.lowerLeft.y; *p++ = -1.0;
    *p++ = button.colour.x; *p++ = button.colour.y; *p++ = button.colour.z; *p++ = button.colour.w;
    *p++ = button.upperRight.x; *p++ = button.upperRight.y; *p++ = -1.0;
    *p++ = button.colour.x; *p++ = button.colour.y; *p++ = button.colour.z; *p++ = button.colour.w;
    *p++ = button.lowerLeft.x; *p++ = button.lowerLeft.y; *p++ = -1.0;
    *p++ = button.colour.x; *p++ = button.colour.y; *p++ = button.colour.z; *p++ = button.colour.w;
    *p++ = button.upperRight.x; *p++ = button.upperRight.y; *p++ = -1.0;
    *p++ = button.colour.x; *p++ = button.colour.y; *p++ = button.colour.z; *p++ = button.colour.w;
    *p++ = button.lowerLeft.x; *p++ = button.upperRight.y; *p++ = -1.0;
    *p++ = button.colour.x; *p++ = button.colour.y; *p++ = button.colour.z; *p++ = button.colour.w;
    *nVertices = (p - pstart) / 7;
    return;
}