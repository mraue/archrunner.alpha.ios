//
//  IWUIMenuPresenter.h
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIMenuPresenter_h
#define theBr1dge_IWUIMenuPresenter_h

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWMath.h"
#include "IWGeometry.h"

#include "IWGPrimitiveBuffer.h"
#include "IWGTextField.h"
#include "IWUIMenuPage.h"

struct _IWUIMenuPresenterData {
    unsigned int nItemsMax, nColumnsItem, nColumnsOption;
    IWPoint2D anchorPoint;
    float aspect;
    float fontHeight;
    float lineHeight;
    IWVector4 color;
    IWGFontMapData *fontMap;
    IWGPrimitiveBufferData triangleBufferData;
    IWGTextFieldData *textFields;
    IWRectangle *buttonRects;
    IWUIMenuPageData * currentPage;
};
typedef struct _IWUIMenuPresenterData IWUIMenuPresenterData;

IWUIMenuPresenterData IWUIMenuPresenterMake(unsigned int nItemsMax,
                                          unsigned int nColumnsItem,
                                          unsigned int nColumnsOption,
                                          IWPoint2D anchorPoint,
                                          float aspect,
                                          float fontHeight,
                                            float lineHeight,
                                          IWVector4 color,
                                          IWGFontMapData *fontMap);
void IWUIMenuPresenterInitTextFields(IWUIMenuPresenterData *menuPresenter, GLfloat *bufferPtr);

void IWIUMenuPresenterPresentMenu(IWUIMenuPresenterData *menuPresenter, IWUIMenuPageData *menuPage);

int IWUIMenuPresenterGetTouch(IWUIMenuPresenterData *menuPresenter, IWPoint2D touchPoint);

void IWUIMenuPresenterPurgeData(IWUIMenuPresenterData *menuPresenter);

#endif
