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

IWUIMenuItemData IWUIMenuItemDataMake(enum IWUIMENUITEM_ITEM_TYPE type,
                                      char* title,
                                      char *optionsString,
                                      unsigned short currentOptionSelected,
                                      IWUIMenuPageData *subMenu,
                                      unsigned short actionID)
{
    IWUIMenuItemData menuItem;
    menuItem.type = type;
    strncpy(menuItem.title, title, IWUIMENUITEMDATA_STRING_LENGTH_MAX - 1);
    strlcpy(menuItem.optionsString, optionsString, IWUIMENUITEMDATA_STRING_LENGTH_MAX - 1);
    menuItem.currentOptionSelected = currentOptionSelected;
    menuItem.subMenu = subMenu;
    menuItem.actionID = actionID;
    return menuItem;
}