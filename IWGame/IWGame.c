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
    gdPlayerData = IWPlayerDataMake(IWVector3Make(1.6, 1.6, 1.6),
                                    IWVector3Normalize(IWVector3Make(-1.0, -1.0, -1.0)),
                                    IWVector3Make(0.0, 1.0, 0.0));
    return;
}

void IWGameUpdate(float timeSinceLastUpdate)
{
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton, gdIsTouched, gdTouchPoint)) {
        gdResetControllerPosition = true;
    }
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton2, gdIsTouched, gdTouchPoint)) {
        gdPlayerData.position = IWVector3Make(1.6, 1.6, 1.6);
        gdPlayerData.direction = IWVector3Normalize(IWVector3Make(-1., -1., -1.));
        gdPlayerData.up = IWVector3Make(0, 1., 0);
    }
    
    if (!gdRectangleButton.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton.colorTransition, timeSinceLastUpdate);
        gdRectangleButton.color = gdRectangleButton.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);

        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, gdRectangleButton.triangleBufferSize * sizeof(GLfloat), gdRectangleButton.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArrayOES(0);
    }
    if (!gdRectangleButton2.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton2.colorTransition, timeSinceLastUpdate);
        gdRectangleButton2.color = gdRectangleButton2.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton2);
        
        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, gdRectangleButton.triangleBufferSize * sizeof(GLfloat), gdRectangleButton2.triangleBufferSize * sizeof(GLfloat), gdRectangleButton2.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArrayOES(0);
    }
    return;
}