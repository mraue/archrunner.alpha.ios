//
//  IWFileTools.c
//  theBr1dge
//
//  Created by Martin Raue on 1/17/13.
//  Copyright (c) 2013 Martin Raue. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

# define FILETOSTRING_FILESIZE_MAX 1024 * 1024 // 1 MB me hopes

const char* IWFileToolsReadFileToString(const char *filename)
{
    long f_size;
    char* code = "";
    size_t code_s, result;
    FILE* fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    f_size = ftell(fp);
    if (f_size > FILETOSTRING_FILESIZE_MAX) {
        printf("ERROR IWFileToolsReadFileToString: file size exceeds maximum allowed file size of %d kB\n",
               FILETOSTRING_FILESIZE_MAX / 1024);
        fclose(fp);
        return code;
    } else {
        fseek(fp, 0, SEEK_SET);
        code_s = sizeof(char) * f_size;
        code = malloc(code_s);
        result = fread(code, 1, f_size, fp);
        fclose(fp);
        return code;
    }
}