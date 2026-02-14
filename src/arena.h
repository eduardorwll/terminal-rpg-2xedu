#if !defined(ARENA_H)
#define ARENA_H

#include "types.h"

typedef struct Arena Arena;

struct Arena
{
	uchar *memory;
	uint capacity;
	uint top;
};

void arenaMalloc(Arena *arena, uint capacity);
void arenaFree(Arena *arena);
void *arenaPush(Arena *arena, uint len, uint alignment);

#endif /* !defined(ARENA_H) */
