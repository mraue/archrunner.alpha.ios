//
//  IWGFontMapEntry.c
//  theBr1dge
//
//  Created by Martin Raue on 2/6/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGFontMapEntry.h"

#include <stdio.h>
#include <stdlib.h>

IWGFontMapEntryData IWGFontMapEntryMakeEmpty()
{
    IWGFontMapEntryData fontMapEntry = {
        false, {0.0, 0.0}, {0.0 , 0.0}
    };
    return fontMapEntry;
}

IWGFontMapEntryData IWGFontMapEntryMake(IWVector2 lowerLeft, IWVector2 upperRight)
{
    IWGFontMapEntryData fontMapEntry = {
        true, lowerLeft, upperRight
    };
    return fontMapEntry;
}

GLfloat* IWGFontMapEntryToTriangles(IWGFontMapEntryData *fontMapEntry,
                                    IWVector2 lowerLeft,
                                    IWVector2 upperRight,
                                    IWVector4 color)
{
    GLfloat *p = malloc(sizeof(GLfloat) * 6 * 9);
    float z = 0.0;
    //
    *p++ = lowerLeft.x; *p++ = lowerLeft.y;  *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    *p++ = fontMapEntry->lowerLeft.x; *p++ = fontMapEntry->lowerLeft.y;
    //
    *p++ = upperRight.x; *p++ = lowerLeft.y;  *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    *p++ = fontMapEntry->upperRight.x; *p++ = fontMapEntry->lowerLeft.y;
    //
    *p++ = upperRight.x; *p++ = upperRight.y;  *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    *p++ = fontMapEntry->upperRight.x; *p++ = fontMapEntry->upperRight.y;
    //
    //
    *p++ = lowerLeft.x; *p++ = lowerLeft.y;  *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    *p++ = fontMapEntry->lowerLeft.x; *p++ = fontMapEntry->lowerLeft.y;
    //
    *p++ = upperRight.x; *p++ = upperRight.y;  *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    *p++ = fontMapEntry->upperRight.x; *p++ = fontMapEntry->upperRight.y;
    //
    *p++ = lowerLeft.x; *p++ = upperRight.y;  *p++ = z;
    *p++ = color.x; *p++ = color.y; *p++ = color.z; *p++ = color.w;
    *p++ = fontMapEntry->lowerLeft.x; *p++ = fontMapEntry->upperRight.y;
    //
    return p;
}