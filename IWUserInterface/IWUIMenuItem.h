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
#include "IWUIMenuItemType.h"

struct _IWUIMenuPageData;

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
                                      const char *title,
                                      const char *optionsString,
                                      unsigned short currentOptionSelected,
                                      struct _IWUIMenuPageData *subMenu,
                                      unsigned short actionID);

#endif
