//
//  IWUIElement.h
//  theBr1dge
//
//  Created by Martin Raue on 1/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIElement_h
#define theBr1dge_IWUIElement_h

#include <stdio.h>

#include "IWOpenGL.h"

#include "IWMath.h"
#include "IWGeometry.h"
#include "IWGPrimitiveBuffer.h"

struct _IWUIElementData {
    IWRectangle rectangle;// [0.0 - 1.0]
    IWGPrimitiveBufferData triangleBuffer;
    IWGPrimitiveBufferData lineBuffer;
};
typedef struct _IWUIElementData IWUIElementData;

IWUIElementData IWUIElementMakeEmpty();

IWUIElementData IWUIElementMakeCircle(IWPoint2D centerPoint,
                                      float radiusX,
                                      IWVector4 color,
                                      float aspectRatio,
                                      unsigned int nDivisions,
                                      GLfloat* bufferPointer);

IWUIElementData IWUIElementMakeArch(IWPoint2D centerPoint,
                                    float radiusX,
                                    IWVector4 color,
                                    float aspectRatio,
                                    unsigned int nDivisions,
                                    GLfloat* bufferPointer,
                                    float thetaMinRad,
                                    float thetaMaxRad);

IWUIElementData IWUIElementMakeCubeSymbol(IWRectangle rectangle,
                                          float lidFraction,
                                          IWVector4 lightColor,
                                          IWVector4 darkColor,
                                          GLfloat *bufferPointer);

#endif
