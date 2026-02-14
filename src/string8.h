#if !defined(STRING8_H)
#define STRING8_H

#include <stdbool.h>

#include "types.h"

typedef struct String8 String8;

struct String8
{
	uint len;
	char *buf;
};

#define S8 string8FromCstr

String8 string8FromCstr(char *str);
bool string8Eq(String8 a, String8 b);

#endif /* !defined(STRING8_H) */
