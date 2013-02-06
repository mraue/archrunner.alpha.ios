//
//  IWGTextLine.h
//  theBr1dge
//
//  Created by Martin Raue on 2/6/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWGTextLine_h
#define theBr1dge_IWGTextLine_h

#include "IWMath.h"
#include "IWGeometry.h"
#include "IWGPrimitiveBuffer.h"
#include "IWGFontMap.h"

typedef struct {
    const char* text;
    IWPoint start;
    float size;
    float aspect;
    IWVector4 color;
    IWGPrimitiveBufferData triangleBufferData;
    
} IWGTextLineData;

IWGTextLineData IWGTextLineDataMake(const char* text,
                                    IWPoint start,
                                    float size,
                                    IWVector4 color,
                                    float aspect,
                                    IWGFontMapData *fontMap);

#endif
