//
//  IWUIMenuPage.c
//  theBr1dge
//
//  Created by Martin Raue on 2/19/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWUIMenuPage.h"

#include <stdio.h>
#include <stdlib.h>

IWUIMenuPageData IWUIMenuPageDataMake(const char* title,
                                      IWUIMenuItemData *items,
                                      unsigned int nItems,
                                      bool isActive,
                                      struct _IWUIMenuPageData *superMenu)
{
    IWUIMenuPageData menuPage = {
        title,
        items,
        nItems,
        isActive,
        superMenu
    };
    return menuPage;
}

void IWUIMenuPageDataSet(IWUIMenuPageData *menuPage,
                         const char* title,
                         struct _IWUIMenuItemData *items,
                         unsigned int nItems,
                         bool isActive,
                         struct _IWUIMenuPageData *superMenu)
{
    menuPage->title = title;
    menuPage->items = items;
    menuPage->nItems = nItems;
    menuPage->isActive = isActive;
    menuPage->superMenu = superMenu;
    return;
}

void IWUIMenuPageAddItem(IWUIMenuPageData *menuPage,
                         enum IWUIMENUITEM_ITEM_TYPE type,
                         char* title,
                         char *optionsString,
                         unsigned short currentOptionSelected,
                         IWUIMenuPageData *subMenu,
                         unsigned short actionID)
{
    menuPage->nItems += 1;
    IWUIMenuItemData *newItems = malloc(menuPage->nItems * sizeof(IWUIMenuItemData));
    for (unsigned short i = 0; i < menuPage->nItems - 1; i++) {
        newItems[i] = menuPage->items[i];
    }
    newItems[menuPage->nItems - 1] = IWUIMenuItemDataMake(type, title, optionsString, currentOptionSelected, subMenu, actionID);
    free(menuPage->items);
    menuPage->items = newItems;
}

void IWUIMenuPagePurgeData(IWUIMenuPageData *menuPage)
{
    free(menuPage->items); menuPage->items = NULL;
}