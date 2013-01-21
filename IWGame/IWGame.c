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
    gdPlayerData = IWPlayerDataMake(IWVector3Make(1.6, 1.0, 1.6),
                                    IWVector3Normalize(IWVector3Make(-1.0, 0.0, -1.0)),
                                    IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0)));
    return;
}

void IWGameUpdate(float timeSinceLastUpdate)
{
    // BEGIN TESTING
    glBindVertexArrayOES(gdVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, gdVertexBuffer);
    
    double minDist = 0.01;
    minDist *= minDist;
//    IWCubeData* cubeFront = &gdCubeData[0];
//    IWCubeData* cubeBack = &gdCubeData[gdNCubes - 1];
//    int iTest = 0, iBack = gdNCubes;
//    while (cubeFront != cubeBack) {
//        if (cubeFront->isVisible && cubeFront->collisionRadius > 0.0) {
//            if (IWVector3DistanceSquared(gdPlayerData.position, cubeFront->centerPosition)
//                < cubeFront->collisionRadius * cubeFront->collisionRadius) {
//                // Collision detected
//                printf("BOING %d\n", iTest);
//                // Hide cube
//                cubeFront->isVisible = false;
//                while (!cubeBack->isVisible && cubeFront != cubeBack) {
//                    cubeBack--;
//                    iBack--;
//                }
//                if (cubeFront == cubeBack) {
//                    break;
//                } else {
//                    memcpy(cubeFront->triangleBufferData.start,
//                           cubeBack->triangleBufferData.start, cubeBack->triangleBufferData.size * sizeof(GLfloat));
//                    cubeBack->triangleBufferData = cubeFront->triangleBufferData;
//                    cubeFront->triangleBufferData = IWGPrimitiveBufferDataMakeEmpty();
//                }
//                glBufferSubData(GL_ARRAY_BUFFER,
//                                (cubeBack->triangleBufferData.start - cubeBack->triangleBufferData.bufferStart) * sizeof(GLfloat),
//                                cubeBack->triangleBufferData.size * sizeof(GLfloat),
//                                gdCubeData[0].triangleBufferData.start);
//                gdN_VERT -= cubeBack->triangleBufferData.size / 6;
//                printf("%d %d\n", cubeBack->triangleBufferData.start - cubeBack->triangleBufferData.bufferStart, cubeBack->triangleBufferData.size * sizeof(GLfloat));
//                printf("Replace with %d\n", iBack);
//                printf("gdN_VERT = %d\n", gdN_VERT);
//                //glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
//            }
//        }
//        cubeFront++;
//        iTest++;
//        //cubeBack--;
//    }
    
    for (int i = 0; i < gdNCubes; i++) {
        if (gdCubeData[i].isVisible && gdCubeData[i].collisionRadius > 0.0) {
            if (IWVector3DistanceSquared(gdPlayerData.position, gdCubeData[i].centerPosition)
                < gdCubeData[i].collisionRadius * gdCubeData[i].collisionRadius) {
                // Collision detected
                // Hide cube
                gdCubeData[i].isVisible = false;
                if (gdBufferToCubeMapNEntries > 0) {
                    gdBufferToCubeMapNEntries -= 1;
                    unsigned currentBufferID, newCubeID;
                    currentBufferID = gdCubeToBufferMap[i];
                    newCubeID = gdBufferToCubeMap[gdBufferToCubeMapNEntries];
                    if (newCubeID != i) {
                        gdBufferToCubeMap[currentBufferID] = newCubeID;
                        gdCubeToBufferMap[newCubeID] = currentBufferID;
                        glBufferSubData(GL_ARRAY_BUFFER,
                                        gdCubeData[i].triangleBufferData.size * currentBufferID * sizeof(GLfloat),
                                        gdCubeData[i].triangleBufferData.size * sizeof(GLfloat),
                                        gdCubeData[newCubeID].triangleBufferData.start);
                    }
                    printf("BOING %u %u %u\n", i, currentBufferID, newCubeID);
                }
                //glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)
                gdN_VERT -= gdCubeData[i].triangleBufferData.size / 6;
            }
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // END TESTING
    
    
    glBindVertexArrayOES(gdUITriangleVertexArray);
    
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton, gdIsTouched, gdTouchPoint)) {
        if (gdDropCamera) {
            gdDropCamera = false;
        } else {
            gdDropCamera = true;
            gdPlayerDataSave = gdPlayerData;
        }
    }
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton2, gdIsTouched, gdTouchPoint)) {
        gdPlayerData.position = IWVector3Make(1.6, 1.0, 1.6);
        gdPlayerData.direction = IWVector3Normalize(IWVector3Make(-1.0, 0.0, -1.0));
        gdPlayerData.up = IWVector3Normalize(IWVector3Make(0.0, 1.0, 0.0));
    }
    if (IWUIRectangleButtonCheckTouch(&gdRectangleButton3, gdIsTouched, gdTouchPoint)) {
        gdResetControllerPosition = true;
    }
    
    if (!gdRectangleButton.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton.colorTransition, timeSinceLastUpdate);
        gdRectangleButton.color = gdRectangleButton.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton);

        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, gdRectangleButton.triangleBufferSize * sizeof(GLfloat), gdRectangleButton.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArrayOES(0);
    }
    if (!gdRectangleButton2.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton2.colorTransition, timeSinceLastUpdate);
        gdRectangleButton2.color = gdRectangleButton2.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton2);
        
        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, (gdRectangleButton2.triangleBufferStart - gdRectangleButton.triangleBufferStart) * sizeof(GLfloat),
                        gdRectangleButton2.triangleBufferSize * sizeof(GLfloat), gdRectangleButton2.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArrayOES(0);
    }
    if (!gdRectangleButton3.colorTransition.transitionHasFinished) {
        IWColorTransitionUpdate(&gdRectangleButton3.colorTransition, timeSinceLastUpdate);
        gdRectangleButton3.color = gdRectangleButton3.colorTransition.currentColor;
        IWUIRectangleButtonUpdateColorInBuffer(&gdRectangleButton3);
        
        //glBindVertexArrayOES(gdVertexArray2);
        glBindBuffer(GL_ARRAY_BUFFER, gdUITriangleVertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, (gdRectangleButton3.triangleBufferStart - gdRectangleButton.triangleBufferStart)  * sizeof(GLfloat),
                        gdRectangleButton3.triangleBufferSize * sizeof(GLfloat), gdRectangleButton3.triangleBufferStart);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindVertexArrayOES(0);
    }
    glBindVertexArrayOES(0);
    return;
}