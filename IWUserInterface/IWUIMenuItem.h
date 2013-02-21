//
//  IWUIMenuItem.h
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIMenuItem_h
#define theBr1dge_IWUIMenuItem_h

#define IWUIMENUITEMDATA_STRING_LENGTH_MAX 255

#include "IWUIMenuPage.h"

struct _IWUIMenuPageData subMenu;

enum IWUIMENUITEM_ITEM_TYPE {
    IWUIMENUITEM_ITEM_TYPE_OPTIONS,
    IWUIMENUITEM_ITEM_TYPE_SUBMENU,
    IWUIMENUITEM_ITEM_TYPE_ACTION,
    IWUIMENUITEM_ITEM_TYPE_OTHER,
    IWUIMENUITEM_ITEM_TYPE_EMPTY,
    IWUIMENUITEM_ITEM_TYPE_N
};

struct _IWUIMenuItemData {
    enum IWUIMENUITEM_ITEM_TYPE type;
    char title[IWUIMENUITEMDATA_STRING_LENGTH_MAX];
    char optionsString[IWUIMENUITEMDATA_STRING_LENGTH_MAX];// divided by \n
    unsigned short currentOptionSelected;
    struct _IWUIMenuPageData *subMenu;
    unsigned short actionID;
};
typedef struct _IWUIMenuItemData IWUIMenuItemData;

IWUIMenuItemData IWUIMenuItemDataMake(enum IWUIMENUITEM_ITEM_TYPE type,
                                      char *title,
                                      char *optionsString,
                                      unsigned short currentOptionSelected,
                                      struct _IWUIMenuPageData *subMenu,
                                      unsigned short actionID);

#endif
