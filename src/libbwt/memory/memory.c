#include <stdlib.h>

#include "memory.h"

arena_t arena_create(const size_t size)
{
	arena_t arena = {0};

	if (size) {
		arena.start = malloc(size);

		if (arena.start) {
			arena.current = arena.start;
			arena.size = size;
		}
	}

	return arena;
}

void *arena_allocate(arena_t *const restrict arena, const size_t size)
{
	if (!size || !arena || !arena->start || (arena->start + arena->size < arena->current + size)) {
		return NULL;
	}

	void *current = arena->current;

	arena->current += size;

	return current;
}

void arena_free(arena_t *const restrict arena, const size_t size)
{
	if (!size || !arena || !arena->start || (arena->start > arena->current - size)) {
		return;
	}

	arena->current -= size;
}

void arena_destroy(arena_t *const restrict arena)
{
	if (!arena) {
		return;
	}

	if (arena->start) {
		free(arena->start);
	}

	arena->start = arena->current = NULL;
	arena->size = 0;
}
