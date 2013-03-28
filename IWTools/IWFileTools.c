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
    for (i = 0; i < (FILETOSTRING_MAX_CHAR_N - 1) && (c = getc(fp)) != EOF; i++) {
        inputStringFull[i] = c;
    }
    fclose(fp);
    if (i == FILETOSTRING_MAX_CHAR_N) {
        printf("ERROR: IWFileToolsReadFileToString maximum string length reached (%d)\n", FILETOSTRING_MAX_CHAR_N);
        return (char*)"";
    } else {
        char* returnString = (char*)malloc(i * sizeof(int));
        if (returnString == NULL) {
            printf("ERROR: IWFileToolsReadFileToString could not allocate memory for return string\n");
            return (char*)"";
        }
        inputStringFull[i] = '\0';
        strncpy(returnString, inputStringFull, i + 1);
        return returnString;
    }
}