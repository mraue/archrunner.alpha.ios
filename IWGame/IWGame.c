//
//  IWGame.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "IWGame.h"

#include "IWMath.h"
#include "IWController.h"

#include "IWGameData.h"

void IWGameSetup(void)
{
    gdPlayerData = IWPlayerDataMake(IWVector3Make(0.0, 0.0, 0.0),
                                    IWVector3Normalize(IWVector3Make(0.0, 0.0, 1.0)),
                                    IWVector3Make(0.0, 1.0, 0.0));
    return;
}

void IWGameUpdate(float timeSinceLastUpdate)
{
    if (!gdRectangleButton.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton.colorTransition, timeSinceLastUpdate);
        gdRectangleButton.color = gdRectangleButton.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);

        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdVertexBuffer2);
        glBufferSubData(GL_ARRAY_BUFFER, 0, gdRectangleButton.memSize * sizeof(GLfloat), gdRectangleButton.memStartPtr);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArrayOES(0);
    } else {
        IWColorTransition colorTransition = {
            gdRectangleButton.colorTransition.endColor,
            gdRectangleButton.colorTransition.startColor,
            gdRectangleButton.colorTransition.endColor,
            gdRectangleButton.colorTransition.transitionTime,
            0.0, false
        };
        gdRectangleButton.colorTransition = colorTransition;
    }
    return;
}