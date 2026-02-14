#include "string8.h"

#include <stdbool.h>
#include <string.h>

String8 string8FromCstr(char *str)
{
	String8 s8 = {0};
	s8.len = strlen(str);
	s8.buf = str;
	return s8;
}

bool string8Eq(String8 a, String8 b)
{
	uint i = 0;

	if (a.len != b.len) {
		return false;
	}

	for (i = 0; i < a.len; i++) {
		if (a.buf[i] != b.buf[i]) {
			return false;
		}
	}

	return true;
}
