//
//  IWScorePresenter.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 2/21/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWScorePresenter.h"

#include <stdio.h>
#include <stdlib.h>

//#define BUFFER_OFFSET(i) ((char *)NULL + (i))

IWScorePresenterData IWScorePresenterMake(float timePerRow,
                                          IWPoint2D anchorPoint,
                                          float aspect,
                                          float fontHeight,
                                          IWVector4 color,
                                          IWGFontMapData *fontMap)
{
    IWScorePresenterData scorePresenter;
    scorePresenter.timePerRow = timePerRow;
    scorePresenter.currentTime = 0.0;
    scorePresenter.hasFinished = false;
    scorePresenter.multiBuffer = IWGRingBufferGen();
    scorePresenter.dataBufferSize = (1 * 8 + 6 * 8 + 6 * 8) * 6 * 9;
    scorePresenter.dataBufferStart = malloc(scorePresenter.dataBufferSize * sizeof(GLfloat));
    scorePresenter.titleTextField = IWGTextFieldMake(anchorPoint, IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                                                           1, 8, aspect, "SCORE",
                                                           fontHeight * 1.6, 0.0, IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                                                           color, fontMap,
                                                           scorePresenter.dataBufferStart);
    anchorPoint.y -= fontHeight * 1.8;
    scorePresenter.descriptionTextField = IWGTextFieldMake(anchorPoint, IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                                                           6, 8, aspect, "",
                                                           fontHeight, 0.0, IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                                                           color, fontMap,
                                                           scorePresenter.dataBufferStart
                                                           + scorePresenter.titleTextField.triangleBufferData.size);
    anchorPoint.x += IWGTextFieldGetCharacterWidth(&scorePresenter.descriptionTextField) * 8;
    scorePresenter.scoreTextField = IWGTextFieldMake(anchorPoint, IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                                                    6, 8, aspect, "",
                                                    fontHeight, 0.0, IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                                    color, fontMap,
                                                    scorePresenter.dataBufferStart
                                                    + scorePresenter.titleTextField.triangleBufferData.size
                                                    + scorePresenter.descriptionTextField.triangleBufferData.size);
    return scorePresenter;
}

void IWScorePresenterUpdate(IWScorePresenterData *scorePresenter,
                            IWScoreCounterData *scoreCounter,
                            float timeSinceLastUpdate)
{
    scorePresenter->currentTime += timeSinceLastUpdate;
    scorePresenter->hasFinished = false;
    char s[255];
    if (scorePresenter->currentTime < scorePresenter->timePerRow) {
        IWGTextFieldSetText(&scorePresenter->descriptionTextField, "Time");
        sprintf(s, "%u", (unsigned int)(IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_TIME)
                * scorePresenter->currentTime / scorePresenter->timePerRow));
        IWGTextFieldSetText(&scorePresenter->scoreTextField, s);
    } else if (scorePresenter->currentTime < scorePresenter->timePerRow * 2.0) {
        IWGTextFieldSetText(&scorePresenter->descriptionTextField, "Time\nCubes");
        sprintf(s, "%u\n%u",
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_TIME),
                (unsigned int)(IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_CUBES)
                               * (scorePresenter->currentTime - scorePresenter->timePerRow) / scorePresenter->timePerRow));
        IWGTextFieldSetText(&scorePresenter->scoreTextField, s);
    } else if (scorePresenter->currentTime < scorePresenter->timePerRow * 3.0) {
        IWGTextFieldSetText(&scorePresenter->descriptionTextField, "Time\nCubes\nDistance");
        sprintf(s, "%u\n%u\n%u",
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_TIME),
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_CUBES),
                (unsigned int)(IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_DISTANCE)
                               * (scorePresenter->currentTime - 2.0 * scorePresenter->timePerRow) / scorePresenter->timePerRow));
        IWGTextFieldSetText(&scorePresenter->scoreTextField, s);
    } else if (scorePresenter->currentTime < scorePresenter->timePerRow * 4.0) {
        IWGTextFieldSetText(&scorePresenter->descriptionTextField, "Time\nCubes\nDistance\n--------\nTotal\n========");
        sprintf(s, "%u\n%u\n%u\n--------\n%u\n========",
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_TIME),
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_CUBES),
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_DISTANCE),
                (unsigned int)(scoreCounter->score
                               * (scorePresenter->currentTime - 3.0 * scorePresenter->timePerRow) / scorePresenter->timePerRow));
        IWGTextFieldSetText(&scorePresenter->scoreTextField, s);
    } else {
        IWGTextFieldSetText(&scorePresenter->descriptionTextField, "Time\nCubes\nDistance\n--------\nTotal\n========");
        sprintf(s, "%u\n%u\n%u\n--------\n%u\n========",
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_TIME),
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_CUBES),
                IWScoreCounterGetScore(scoreCounter, IWSCORECOUNTER_SCORE_TYPE_DISTANCE),
                (unsigned int)(scoreCounter->score));
        IWGTextFieldSetText(&scorePresenter->scoreTextField, s);
        scorePresenter->hasFinished = true;
    }
    
    
    IWGRingBufferSubData(&scorePresenter->multiBuffer, 0,
                          scorePresenter->dataBufferSize * sizeof(GLfloat),
                          scorePresenter->dataBufferStart, true);
    return;
}

void IWScorePresenterFillVBO(IWScorePresenterData *scorePresenter,
                             GLuint positionSlot,
                             GLuint colorSlot,
                             GLuint textureOffsetSlot,
                             GLuint textureHandlerId,
                             GLvoid* fontMapTextureData)
{
    // Fill buffers
    for (unsigned int i = 0; i < IWGMULTIBUFFER_MAX; i++) {
        
        IWGRingBufferBind(&scorePresenter->multiBuffer, i);
        
        scorePresenter->multiBuffer.nVertices[i] = scorePresenter->dataBufferSize / 9;
        
        glBindTexture(GL_TEXTURE_2D, textureHandlerId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, fontMapTextureData);
        //glGenerateMipmap(GL_TEXTURE_2D);
        
        glBufferData(GL_ARRAY_BUFFER,
                     scorePresenter->dataBufferSize * sizeof(GLfloat),
                     scorePresenter->dataBufferStart,
                     GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(positionSlot);
        glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(0));
        glEnableVertexAttribArray(colorSlot);
        glVertexAttribPointer(colorSlot, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(textureOffsetSlot);
        glVertexAttribPointer(textureOffsetSlot, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), BUFFER_OFFSET(7 * sizeof(GLfloat)));
    }
    return;
}

void IWScorePresenterRender(IWScorePresenterData *scorePresenter)
{
    IWGRingBufferBindCurrentDrawBuffer(&scorePresenter->multiBuffer);
    
    glDrawArrays(GL_TRIANGLES, 0, scorePresenter->multiBuffer.nVertices[scorePresenter->multiBuffer.currentDrawBuffer]);
    
    glBindVertexArrayOES(0);
    
    IWGRingBufferSwitchBuffer(&scorePresenter->multiBuffer);
    
    return;
}

void IWScorePresenterPurgeData(IWScorePresenterData *scorePresenter)
{
    IWGRingBufferDealloc(&scorePresenter->multiBuffer);
    free(scorePresenter->dataBufferStart);
    return;
}