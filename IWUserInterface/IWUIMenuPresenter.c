//
//  IWUIMenuPresenter.c
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIMenuPresenter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

IWUIMenuPresenterData IWUIMenuPresenterMake(unsigned int nItemsMax,
                                            unsigned int nColumnsItem,
                                            unsigned int nColumnsOption,
                                            IWPoint2D anchorPoint,
                                            float aspect,
                                            float fontHeight,
                                            float lineHeight,
                                            IWVector4 color,
                                            IWGFontMapData *fontMap)
{
    IWUIMenuPresenterData menuPresenter = {
        nItemsMax,
        nColumnsItem,
        nColumnsOption,
        anchorPoint,
        aspect,
        fontHeight,
        lineHeight,
        color,
        fontMap,
        IWGPrimitiveBufferDataMakeEmpty(),
        NULL, NULL, NULL
    };
    menuPresenter.triangleBufferData.size = ((menuPresenter.nItemsMax + 1) * menuPresenter.nColumnsItem + menuPresenter.nItemsMax * menuPresenter.nColumnsOption) * 6 * 9;
    return menuPresenter;
}

void IWUIMenuPresenterInitTextFields(IWUIMenuPresenterData *menuPresenter, GLfloat *bufferPtr)
{
    free(menuPresenter->textFields); menuPresenter->textFields = NULL;
    free(menuPresenter->buttonRects); menuPresenter->buttonRects = NULL;
    
    menuPresenter->triangleBufferData.bufferStartCPU = bufferPtr;
    menuPresenter->textFields = (IWGTextFieldData*)malloc((menuPresenter->nItemsMax * 2 + 1) * sizeof(IWGTextFieldData));
    menuPresenter->buttonRects = malloc(menuPresenter->nItemsMax * sizeof(IWRectangle));
    
    IWPoint2D startPosition = menuPresenter->anchorPoint;
    
    // Create menu title text field
    menuPresenter->textFields[0] = IWGTextFieldMake(startPosition, IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                                                    1, menuPresenter->nColumnsItem, menuPresenter->aspect, "",
                                                    menuPresenter->fontHeight * 1.8, 0.0,
                                                    IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                                                    menuPresenter->color,
                                                    menuPresenter->fontMap,
                                                    bufferPtr);
    
    bufferPtr += menuPresenter->textFields[0].triangleBufferData.size;
    startPosition.y -= menuPresenter->fontHeight * 2.2;
    
    for (unsigned int i = 0; i < menuPresenter->nItemsMax * 2; i += 2) {
        menuPresenter->textFields[i + 1] = IWGTextFieldMake(startPosition, IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                                                            1, menuPresenter->nColumnsItem, menuPresenter->aspect, "",
                                                            menuPresenter->fontHeight, 0.0,
                                                            IWGTEXT_HORIZONTAL_ALIGNMENT_LEFT,
                                                            menuPresenter->color,
                                                            menuPresenter->fontMap,
                                                            bufferPtr);
        bufferPtr += menuPresenter->textFields[i + 1].triangleBufferData.size;
        startPosition.x += IWGTextFieldGetCharacterWidth(&menuPresenter->textFields[1]) * menuPresenter->nColumnsItem;
        menuPresenter->textFields[i + 2] = IWGTextFieldMake(startPosition, IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT,
                                                            1, menuPresenter->nColumnsOption, menuPresenter->aspect, "",
                                                            menuPresenter->fontHeight, 0.0,
                                                            IWGTEXT_HORIZONTAL_ALIGNMENT_RIGHT,
                                                            menuPresenter->color,
                                                            menuPresenter->fontMap,
                                                            bufferPtr);
        bufferPtr += menuPresenter->textFields[i + 2].triangleBufferData.size;
        startPosition.y -= menuPresenter->fontHeight * menuPresenter->lineHeight;
        startPosition.x = menuPresenter->anchorPoint.x;
    }
    IWVector2 dimensions = {
        IWGTextFieldGetCharacterWidth(&menuPresenter->textFields[1]) * menuPresenter->nColumnsItem * menuPresenter->nColumnsOption / 2.0,
        menuPresenter->fontHeight * menuPresenter->lineHeight / 2.0
    };
    float xstart = (menuPresenter->anchorPoint.x + 1.0) / 2.0;
    float ystart = (menuPresenter->anchorPoint.y - menuPresenter->fontHeight * 2.2 + 1.0) / 2.0;
    for (unsigned int i = 0; i < menuPresenter->nItemsMax; i++) {
        menuPresenter->buttonRects[i] =
            IWRectangleMakeFromAnchorAndDimensions(IWVector2Make(xstart, ystart - i * dimensions.y),
                                                   dimensions,
                                                   IWGEOMETRY_ANCHOR_POSITION_UPPER_LEFT);
    }
    
    return;
}

void IWIUMenuPresenterPresentMenu(IWUIMenuPresenterData *menuPresenter, IWUIMenuPageData *menuPage)
{
    if (menuPage->nItems > menuPresenter->nItemsMax) {
        printf("ERROR IWUIMenuPresenterPresentMenu: Menu page has too many items\n");
        return;
    }
    
    IWGTextFieldSetText(&menuPresenter->textFields[0], menuPage->title);
    
    unsigned int n = 1;
    for (unsigned int i = 0; i < menuPage->nItems; i++) {
        IWGTextFieldSetText(&menuPresenter->textFields[n], menuPage->items[i].title);
        n++;
        if (menuPage->items[i].type == IWUIMENUITEM_ITEM_TYPE_OPTIONS) {
            unsigned int optionCounter = 0;

            if (strlen(menuPage->items[i].optionsString) > 100) {
                printf("ERROR IWIUMenuPresenterPresentMenu: Option string too long\n");
                return;
            }
            char *searchString = malloc(100 * sizeof(char));            
            strncpy(searchString, menuPage->items[i].optionsString, 100);
            char *strTok = NULL;
            while ((optionCounter <= menuPage->items[i].currentOptionSelected)
                   && (strTok = strtok(optionCounter == 0 ? searchString : NULL, "\n"))) {
                optionCounter++;
            }
            if (strTok)
                IWGTextFieldSetText(&menuPresenter->textFields[n], strTok);
            free(searchString);
        }
        n++;
    }
    
    menuPresenter->currentPage = menuPage;
    
    return;
}

int IWUIMenuPresenterGetTouch(IWUIMenuPresenterData *menuPresenter, IWPoint2D touchPoint)
{
    for (int i = 0; i < menuPresenter->nItemsMax; i++) {
        if (IWPointInRectangle(touchPoint, menuPresenter->buttonRects[i])) {
            return i;
        }
    }
    return -1;
}

void IWUIMenuPresenterPurgeData(IWUIMenuPresenterData *menuPresenter)
{
    //free(menuPresenter->triangleBufferData.bufferStartCPU);
    //menuPresenter->triangleBufferData.bufferStartCPU = NULL;
    free(menuPresenter->textFields);
    menuPresenter->textFields = NULL;
    free(menuPresenter->buttonRects);
    menuPresenter->buttonRects = NULL;
}