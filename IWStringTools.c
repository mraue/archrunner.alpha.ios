//
//  IWStringTools.c
//  ArchRunnerAlpha
//
//  Created by Martin Raue on 4/5/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include "IWStringTools.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IWMath.h"

#define IWSTRING_MAX_TOKEN_LENGTH 100

char ** IWStringToolsSplitString(char str[], const char* div, unsigned int *nSplit) {
    
    char ** res = NULL;
    char *  p    = strtok (str, div);
    int n_spaces = 0;
    
    *nSplit = 0;
    
    /* split string and append tokens to 'res' */
    
    while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces);
        
        //if (res == NULL)
        //    exit (-1); /* memory allocation failed */
        
        size_t nChar = IW_MIN(strlen(p), IWSTRING_MAX_TOKEN_LENGTH) + 1;

        char* np = (char*)malloc(sizeof(char*) * nChar);
        strncpy(np, p, nChar);
        
        res[n_spaces-1] = np;
        
        p = strtok (NULL, " ");
    }
    
    /* realloc one extra element for the last NULL */
    
    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;
    
    *nSplit = n_spaces;
    
//    /* free the memory allocated */
//    for (int i = 0; i < (n_spaces+1); ++i)
//        printf ("res[%d] = %s\n", i, res[i]);
    
    return res;
    
    //free (res);
}