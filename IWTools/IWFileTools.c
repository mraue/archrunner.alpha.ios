//
//  IWFileTools.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define FILETOSTRING_MAX_CHAR_N 30000

char* IWFileToolsReadFileToString(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    char inputStringFull[FILETOSTRING_MAX_CHAR_N];
    int c, i;
    for (i = 0; i < FILETOSTRING_MAX_CHAR_N && (c = getc(fp)) != EOF; i++) {
        inputStringFull[i] = c;
    }
    if (i == FILETOSTRING_MAX_CHAR_N) {
        printf("ERROR: IWFileToolsReadFileToString maximum string length reached (%d)\n", FILETOSTRING_MAX_CHAR_N);
        fclose(fp);
        return "";
    } else {
        inputStringFull[i] = '\0';
        char* returnString = malloc(i * sizeof(int));
        strncpy(returnString, inputStringFull, i + 1);
        fclose(fp);
        return returnString;
    }
}