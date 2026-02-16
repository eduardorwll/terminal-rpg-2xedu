#if !defined(UTIL_H)
#define UTIL_H

#include "string8.h"

#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int readWholeFile(char *filepath, String8 *str);

#endif /* !defined(UTIL_H) */
