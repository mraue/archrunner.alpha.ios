//
//  IWGFontMapEntry.h
//  theBr1dge
//
//  Created by Martin Raue on 2/6/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGFontMapEntry_h
#define theBr1dge_IWGFontMapEntry_h

#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"

typedef struct {
    bool hasData;
    IWVector2 lowerLeft;
    IWVector2 upperRight;
} IWGFontMapEntryData;

IWGFontMapEntryData IWGFontMapEntryMakeEmpty();
IWGFontMapEntryData IWGFontMapEntryMake(IWVector2 lowerLeft, IWVector2 upperRight);

float IWGFontMapEntryGetAspectRatio(IWGFontMapEntryData* fontMapEntry);

unsigned int IWGFontMapEntryToTriangles(GLfloat *p,
                                        IWGFontMapEntryData *fontMapEntry,
                                        IWVector2 lowerLeft,
                                        IWVector2 upperRight,
                                        IWVector4 color);

#endif
