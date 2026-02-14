#if !defined(UTIL_H)
#define UTIL_H

#include "string8.h"

int readWholeFile(char *filepath, String8 *str);
int writeToFile(String8 str, char *filepath);

#endif /* !defined(UTIL_H) */
