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

struct String8Array
{
	String8 strings[10];
	int count;
};

typedef struct String8Array String8Array;

#define S8 string8FromCstr
#define S8Arr S8ArrayFromCstrArray

String8Array S8ArrayFromCstrArray(char *strArray[], int strArrayLen);
String8 string8FromCstr(char *str);
bool string8Eq(String8 a, String8 b);

#endif /* !defined(STRING8_H) */
