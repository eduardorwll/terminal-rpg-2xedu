#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "arena.h"

void arenaMalloc(Arena *arena, uint capacity)
{
	arena->memory = malloc(capacity);
	assert(arena->memory != NULL);
}

void arenaFree(Arena *arena)
{
	free(arena->memory);
}

void *arenaPush(Arena *arena, uint len, uint alignment)
{
	uint padding = -arena->top % alignment;
	void *ptr = NULL;

	assert(padding <= alignment);
	assert(arena->top + padding + len < arena->capacity);

	if (padding == alignment) {
		padding = 0;
	}

	arena->top += padding;
	ptr = &arena->memory[arena->top];
	arena->top += len;

	return ptr;
}
