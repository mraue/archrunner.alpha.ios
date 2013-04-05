//
//  IWUIMenuItem.c
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIMenuItem.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "IWStringTools.h"

IWUIMenuItemData IWUIMenuItemDataMake(enum IWUIMENUITEM_ITEM_TYPE type,
                                      const char* title,
                                      const char* optionsString,
                                      unsigned short currentOptionSelected,
                                      unsigned short subMenuId,
                                      unsigned short id_)
{
    IWUIMenuItemData menuItem;
    menuItem.type = type;
    menuItem.title = title;
    menuItem.optionsString = optionsString;
    menuItem.options = NULL;
    if (strcmp(optionsString, "") != 0
        && strlen(optionsString) < 200) {
        char copyOfOptionsString[200];
        strcpy(copyOfOptionsString, optionsString);
        menuItem.options = IWStringToolsSplitString(copyOfOptionsString, "\n", &menuItem.nOptions);
    }
    menuItem.currentOptionSelected = currentOptionSelected;
    menuItem.subMenuId = subMenuId;
    menuItem.id_ = id_;
    return menuItem;
}

void IWUIMenuItemPurgeData(IWUIMenuItemData* menuItem)
{
    if (menuItem->options) {
        for (int i = 0; i < menuItem->nOptions; i++) {
            free(menuItem->options[i]);
        }
        free(menuItem->options);
        menuItem->options = NULL;
    }
    return;
}