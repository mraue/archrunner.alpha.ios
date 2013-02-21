//
//  IWUIMenuPage.h
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#ifndef theBr1dge_IWUIMenuPage_h
#define theBr1dge_IWUIMenuPage_h

#include <stdbool.h>

#include "IWUIMenuItem.h"

// Forward declarations
struct _IWUIMenuItemData;
enum IWUIMENUITEM_ITEM_TYPE;

struct _IWUIMenuPageData {
    const char* title;
    struct _IWUIMenuItemData *items;
    unsigned int nItems;
    bool isActive;
    struct _IWUIMenuPageData *superMenu;
};
typedef struct _IWUIMenuPageData IWUIMenuPageData;

IWUIMenuPageData IWUIMenuPageDataMake(const char* title,
                                      struct _IWUIMenuItemData *items,
                                      unsigned int nItems,
                                      bool isActive,
                                      struct _IWUIMenuPageData *superMenu);


void IWUIMenuPageDataSet(IWUIMenuPageData *menuPage,
                         const char* title,
                         struct _IWUIMenuItemData *items,
                         unsigned int nItems,
                         bool isActive,
                         struct _IWUIMenuPageData *superMenu);

void IWUIMenuPageAddItem(IWUIMenuPageData *menuPage,
                         enum IWUIMENUITEM_ITEM_TYPE type,
                         char* title,
                         char *optionsString,
                         unsigned short currentOptionSelected,
                         IWUIMenuPageData *subMenu,
                         unsigned short actionID);

void IWUIMenuPagePurgeData(IWUIMenuPageData *menuPage);

#endif
