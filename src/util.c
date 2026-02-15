#include "../include/util.h"

#include <stdio.h>
#include <stdlib.h>

int readWholeFile(char *filepath, String8 *str)
{
	FILE *fp = fopen(filepath, "r");

	if (fp == NULL)
	{
		return 1;
	}

	if (fseek(fp, 0L, SEEK_END) != 0)
	{
		fclose(fp);
		return 1;
	}
	str->len = ftell(fp);
	rewind(fp);

	str->buf = malloc(str->len);
	if (str->buf == NULL)
	{
		fclose(fp);
		return 1;
	}

	if (fread(str->buf, 1, str->len, fp) != str->len)
	{
		free(str->buf);
		fclose(fp);
		return 1;
	}

	fclose(fp);

	return 0;
}

int writeToFile(String8 str, char *filepath)
{
	FILE *fp = fopen(filepath, "w");
	if (fp == NULL)
	{
		return 1;
	}

	if (fwrite(str.buf, 1, str.len, fp) != str.len)
	{
		return 1;
	}

	fclose(fp);

	return 0;
}
