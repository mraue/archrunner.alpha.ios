//
//  IWUISlider.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/25/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUISlider.h"

#include <stdio.h>

IWUISliderData IWUISliderMake(IWRectangle rectangle)
{
    IWUISliderData slider = {
        rectangle, false, {0.0, 0.0}, {0.0, 0.0}
    };
    return slider;
}

IWPoint2D IWUISliderUpdateWithTouch(IWUISliderData* slider, IWPoint2D touch)
{
    if (IWPointInRectangle(touch, slider->rectangle)) {
        if (!slider->isActive) {
            slider->isActive = true;
            slider->lastTouch = slider->firstTouch = touch;
            return IWVector2Make(0.0, 0.0);
        } else {
            IWPoint2D touchTmp = slider->lastTouch;
            slider->lastTouch = touch;
            return IWVector2Substract(touch, touchTmp);
        }
    } else {
        slider->isActive = false;
        return IWVector2Make(0.0, 0.0);
    }
}
void IWUISliderTouchHasEnded(IWUISliderData* slider)
{
    slider->isActive = false;
}
