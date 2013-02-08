//
//  IWGTextField.c
//  theBr1dge
//
//  Created by Martin Raue on 2/8/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGTextField.h"

#include <stdio.h>
#include <string.h>

#include "IWGFontMapEntry.h"

IWGTextFieldData IWGTextFieldMake(IWPoint2D anchorPoint,
                                  IWGEOMETRY_ANCHOR_POSITION anchorPosition,
                                  unsigned int nRows,
                                  unsigned int nColumns,
                                  float aspect,
                                  const char* text,
                                  float fontHeight,
                                  float lineExtraOffset,
                                  IWGTEXT_HORIZONTAL_ALIGNMENT alignment,
                                  IWVector4 color,
                                  IWGFontMapData *fontMap,
                                  GLfloat *bufferStartPtr)
{
    IWGTextFieldData textField = {
        anchorPoint, anchorPosition,
        nRows, nColumns,
        aspect,
        text,
        fontHeight,
        lineExtraOffset,
        alignment,        
        color,
        fontMap,
        IWGPrimitiveBufferDataMake(nRows * nColumns * 6 * 9, 9, NULL, bufferStartPtr, 0, 0, 0, 0, 3, 7, 0),
        false
    };
    if (!bufferStartPtr) {
        textField.triangleBufferData.startCPU = malloc(nRows * nColumns * 6 * 9 * sizeof(GLfloat));
        textField.manageBuffer = true;
    }

    IWGTextFieldSetText(&textField, text);

    return textField;
}

void IWGTextFieldSetText(IWGTextFieldData *textField, const char* text)
{
    textField->text = text;
    
    GLfloat* p = textField->triangleBufferData.startCPU;
    
    IWVector2 lowerLeft, upperRight;
    float xStart = textField->anchorPoint.x;
    switch (textField->anchorPosition) {
        case IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT:
            lowerLeft = upperRight = textField->anchorPoint;
            lowerLeft.y -= textField->fontHeight;
            break;
        case IWGEOMETRY_ANCHOR_POSITION_LOWER_LEFT:
            lowerLeft = upperRight = textField->anchorPoint;
            lowerLeft.y += (textField->nRows - 1) * (textField->fontHeight + textField->lineExtraOffset) + textField->lineExtraOffset;
            upperRight.y = lowerLeft.y + textField->fontHeight;
            break;
        case IWGEOMETRY_ANCHOR_POSITION_UPPER_RIGHT:
            lowerLeft = upperRight = textField->anchorPoint;
            lowerLeft.y -= textField->fontHeight;
            float mWidth = textField->fontHeight * IWGFontMapEntryGetAspectRatio(&textField->fontMap->map['M'])  / textField->aspect;
            xStart = lowerLeft.x = upperRight.x = textField->anchorPoint.x - textField->nColumns * mWidth;
            break;
        default:
            printf("ERROR IWGTextFieldSetText: Unknown anchor position. Using IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT\n");
            lowerLeft = upperRight = textField->anchorPoint;
            lowerLeft.y -= textField->fontHeight;
            break;
    }

    
    char emtpyChar = ' ';
    
    char *textVar = malloc(strlen(text) + 1);
    strcpy(textVar, text);
    char *line = strtok(textVar, "\n");
    
    for (unsigned int y = 0; y < textField->nRows; y++) {
        for (unsigned int x = 0; x < textField->nColumns; x++) {
            char c = emtpyChar;
            if (textField->alignment == IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT
                && (line && x < strlen(line))) {
                c = line[x];
            } else if (line && x >= textField->nColumns - strlen(line)) {
                c = line[x - (textField->nColumns - strlen(line))];
            }
            float aspectTotal = IWGFontMapEntryGetAspectRatio(&textField->fontMap->map[c]) / textField->aspect;
            upperRight.x += textField->fontHeight * aspectTotal;
            p += IWGFontMapEntryToTriangles(p, &textField->fontMap->map[c],
                                            lowerLeft, upperRight, textField->color);
            lowerLeft.x = upperRight.x;
        }
        if (line) {
            line = strtok(NULL, "\n");
        }
        upperRight.y = upperRight.y - textField->fontHeight - textField->lineExtraOffset;
        lowerLeft.y = lowerLeft.y - textField->fontHeight - textField->lineExtraOffset;
        upperRight.x = lowerLeft.x = xStart;
    }
    free(textVar);
    return;
}
