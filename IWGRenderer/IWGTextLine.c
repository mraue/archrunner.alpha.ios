//
//  IWGTextLine.c
//  theBr1dge
//
//  Created by Martin Raue on 2/6/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGTextLine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWGFontMapEntry.h"

IWGTextLineData IWGTextLineDataMake(const char* text,
                                    IWPoint2D start,
                                    float size,
                                    IWVector4 color,
                                    float aspect,
                                    IWGFontMapData *fontMap)
{
    IWGTextLineData textLine;
    textLine.text = text;
    textLine.start = start;
    textLine.size = size;
    textLine.color = color;
    textLine.aspect = aspect;
    
    unsigned int nChar = strlen(text);
    
    GLfloat *p = malloc(nChar * 6 * 9 * sizeof(GLfloat));
    
    textLine.triangleBufferData = IWGPrimitiveBufferDataMake(nChar * 6 * 9, 9, p, p, 0, 0, 0, 0, 3, 7, 0);
    
    IWVector2 lowerLeft, upperRight;
    lowerLeft = upperRight = start;
    upperRight.y += size;
    for (unsigned int i = 0; i < nChar; i++) {
        char c = text[i];
        float aspectTotal = IWGFontMapEntryGetAspectRatio(&fontMap->map[c]) / aspect;
        upperRight.x += size * aspectTotal;
        p += IWGFontMapEntryToTriangles(p, &fontMap->map[c], lowerLeft, upperRight, color);
        lowerLeft.x = upperRight.x;
    }
    
    return textLine;
}
