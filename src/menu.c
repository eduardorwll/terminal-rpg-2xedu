#include <stdio.h>
#include <string.h>

#include "../include/string8.h"

void enumerateOptions(String8Array *options)
{
    int i = 1;

    for (i; i <= options->count; i++)
    {
        printf("\n%d. %.*s", i, options->strings[i - 1].len, options->strings[i - 1].buf);
    }
}

void initMainMenu(void)
{
    char *strOptions[] = {"Jogar"};

    String8Array options = S8Arr(strOptions);

    enumerateOptions(&options);
}