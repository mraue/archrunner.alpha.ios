//
//  IWUIStateBar.c
//  theBr1dge
//
//  Created by Martin Raue on 1/22/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIStateBar.h"

#include <stdio.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

IWUIStateBar IWUIStateBarMake(unsigned int nStates, float *states,
                              IWVector4 *colors, IWRectangle rectangle,
                              IWUI_ORIENTATION orientation, IWUI_DIRECTION direction)
{
    float *_states = malloc(nStates * sizeof(float));
    IWVector4 *_colors = malloc(nStates * sizeof(IWVector4));
    for (int i = 0; i < nStates; i++) {
        _states[i] = states[i];
        _colors[i] = colors[i];
    }
    IWUIStateBar stateBar = {
        nStates, _states, _colors,
        rectangle, orientation, direction,
        IWUIElementMakeEmpty()
        
    };
    return stateBar;
}

size_t IWUIStateBarToTriangles(IWUIStateBar *stateBar)
{
    unsigned int indices[] = {0, 1, 2, 0, 2, 3};
    GLfloat *p = stateBar->uiElementData.triangleBufferStart;
    GLfloat *pstart = p;
    IWVector2 llV = IWVector2AddScalar(IWVector2MultiplyScalar(stateBar->rectangle.lowerLeft, 2.0), -1.0);
    IWVector2 urV = IWVector2AddScalar(IWVector2MultiplyScalar(stateBar->rectangle.upperRight, 2.0), -1.0);
    IWVector2 diffV = IWVector2Substract(urV, llV);
    float z = -0.999;
    if (stateBar->orientation == IWUI_ORIENTATION_HORIZONTAL) {
        IWVector2 points[] = {
            llV, {0.0, 0.0}, {0.0, 0.0}, {llV.x, urV.y}
        };
        for (int i = 0; i < stateBar->nStates; i++) {
            points[1] = IWVector2Make(llV.x + diffV.x * stateBar->states[i], llV.y);
            points[2] = IWVector2Make(llV.x + diffV.x * stateBar->states[i], urV.y);
            for (int j = 0; j < 6; j++) {
                *p++ = points[indices[j]].x; *p++ = points[indices[j]].y; *p++ = z;
                *p++ = stateBar->colors[i].x; *p++ = stateBar->colors[i].y;
                *p++ = stateBar->colors[i].z; *p++ = stateBar->colors[i].w;
            }
            points[0] = points[1];
            points[3] = points[2];
        }
    } else {
        IWVector2 points[] = {
            llV, {urV.x, llV.y}, {0.0, 0.0}, {0.0, 0.0}
        };
        for (int i = 0; i < stateBar->nStates; i++) {
            points[2] = IWVector2Make(urV.x, llV.y + diffV.y * stateBar->states[i]);
            points[3] = IWVector2Make(llV.x, llV.y + diffV.y * stateBar->states[i]);
            for (int j = 0; j < 6; j++) {
                *p++ = points[indices[j]].x; *p++ = points[indices[j]].y; *p++ = z;
                *p++ = stateBar->colors[i].x; *p++ = stateBar->colors[i].y;
                *p++ = stateBar->colors[i].z; *p++ = stateBar->colors[i].w;
            }
            points[0] = points[3];
            points[1] = points[2];
        }
    }
    return (p - pstart);
}