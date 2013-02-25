//
//  IWUISlider.h
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef ArchRunnerAlpha_IWUISlider_h
#define ArchRunnerAlpha_IWUISlider_h

#include <stdbool.h>

#include "IWGeometry.h"
#include "IWMath.h"

typedef struct {
    IWRectangle rectangle;
    bool isActive;
    IWPoint2D firstTouch;
    IWPoint2D lastTouch;
} IWUISliderData;

IWUISliderData IWUISliderMake(IWRectangle rectangle);

IWPoint2D IWUISliderUpdateWithTouch(IWUISliderData* slider, IWPoint2D touch);
void IWUISliderTouchHasEnded(IWUISliderData* slider);

#endif
