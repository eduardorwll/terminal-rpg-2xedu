#include "../include/string8.h"

#include <stdbool.h>
#include <string.h>

String8 string8FromCstr(char *str)
{
	String8 s8 = {0};
	s8.len = strlen(str);
	s8.buf = str;
	return s8;
}

String8Array S8ArrayFromCstrArray(char *strArray[])
{
	/*int strArrayLen = sizeof(char **) / sizeof(strArray[0]);*/
	int strArrayLen = 1;

	String8Array S8Array = {0};

	int c = 0;
	for (; c < strArrayLen; c++)
	{
		S8Array.strings[c] = S8(strArray[c]);
		S8Array.count++;
	}

	return S8Array;
}

bool string8Eq(String8 a, String8 b)
{
	uint i = 0;

	if (a.len != b.len)
	{
		return false;
	}

	for (i = 0; i < a.len; i++)
	{
		if (a.buf[i] != b.buf[i])
		{
			return false;
		}
	}

	return true;
}
