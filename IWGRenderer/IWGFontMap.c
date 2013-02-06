//
//  IWGFontMap.c
//  theBr1dge
//
//  Created by Martin Raue on 2/6/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWGFontMap.h"

#include <stdio.h>
#include <string.h>

#include "IWFileTools.h"

#define IWGFONTMAP_MAX_FILE_ENTRIES_SAVE 1000

IWGFontMapData IWGFontMapMakeEmpty()
{
    IWGFontMapData fontMap;
    fontMap.mapIsLoaded = false;
    for (unsigned int i = 0; i < 127; i++)
        fontMap.map[i] = IWGFontMapEntryMakeEmpty();
    return fontMap;
}

IWGFontMapData IWGFontMapCreateFromFile(const char* filename)
{
    IWGFontMapData fontMap = IWGFontMapMakeEmpty();
    
    char *fileContent = IWFileToolsReadFileToString(filename);
    
    int charID;
    float x1, x2, y1, y2;
    
    unsigned int nFileEntries = 0;
    char *line = NULL;
    
    while ((line = strtok(nFileEntries == 0 ? fileContent : NULL, "\n")) != NULL
           && nFileEntries < IWGFONTMAP_MAX_FILE_ENTRIES_SAVE){
        if (sscanf(line, "%i %f %f %f %f", &charID, &x1, &y1, &x2, &y2) != EOF) {
            if (charID > 0 && charID < 127) {
                fontMap.map[charID].lowerLeft = IWVector2Make(x1, y1);
                fontMap.map[charID].upperRight = IWVector2Make(x2, y2);
                fontMap.map[charID].hasData = true;
            } else {
                printf("ERROR IWGFontMapCreateFromFile: Character id %u out of range", charID);
            }
        }
        nFileEntries++;
    }
    return fontMap;
}
