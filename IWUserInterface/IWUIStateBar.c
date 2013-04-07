//
//  IWUIStateBar.c
//  theBr1dge
//
//  Created by Martin Raue on 1/22/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIStateBar.h"

#include <stdio.h>

#include "IWOpenGL.h"

IWUIStateBar IWUIStateBarMake(unsigned int nStates, float *states,
                              IWVector4 *colors, IWRectangle rectangle,
                              IWUI_ORIENTATION orientation, IWUI_DIRECTION direction)
{
    float *_states = (float*)malloc(nStates * sizeof(float));
    IWVector4 *_colors = (IWVector4*)malloc(nStates * sizeof(IWVector4));
    for (int i = 0; i < nStates; i++) {
        _states[i] = states[i];
        _colors[i] = colors[i];
    }
    IWUIStateBar stateBar = {
        nStates, _states, _colors,
        rectangle, orientation, direction,
        IWGPrimitiveBufferDataMakeEmpty()
        
    };
    return stateBar;
}

size_t IWUIStateBarToTriangles(IWUIStateBar *stateBar)
{
    float *statesSaved;
    IWVector4 *colorsSaved;
    if (stateBar->direction == IWUI_DIRECTION_REVERSE) {
        statesSaved = stateBar->states;
        colorsSaved = stateBar->colors;
        stateBar->states = (float*)malloc((stateBar->nStates + 1) * sizeof(float));
        stateBar->colors = (IWVector4*)malloc((stateBar->nStates + 1) * sizeof(IWVector4));
        stateBar->states[0] = 1.0 - statesSaved[stateBar->nStates - 1];
        stateBar->colors[0] = IWVector4Make(0.0, 0.0, 0.0, 0.0);
        for (unsigned int i = 1; i < stateBar->nStates; i++) {
            stateBar->states[i] = 1.0 - statesSaved[stateBar->nStates - i - 1];
            stateBar->colors[i] = colorsSaved[stateBar->nStates - i];
        }
        stateBar->states[stateBar->nStates] = 1.0;
        stateBar->colors[stateBar->nStates] = colorsSaved[0];
        stateBar->nStates += 1;
    }
    unsigned int indices[] = {0, 1, 2, 0, 2, 3};
    GLfloat *p = stateBar->triangleBufferData.startCPU;
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
    if (stateBar->direction == IWUI_DIRECTION_REVERSE) {
        free(stateBar->states);
        free(stateBar->colors);
        stateBar->states = statesSaved;
        stateBar->colors = colorsSaved;
        stateBar->nStates -= 1;
    }
    return stateBar->triangleBufferData.size = (p - pstart);
}

void IWUIStateBarDeallocData(IWUIStateBar *stateBar)
{
    free(stateBar->colors);
    stateBar->colors = NULL;
    free(stateBar->states);
    stateBar->states = NULL;
    stateBar->nStates = 0;
    return;
}