#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
		const char *start;
		const char *current;
		size_t size;
} arena_t;

arena_t arena_create(const size_t size);
void *arena_allocate(arena_t *const restrict arena, const size_t size);
void arena_free(arena_t *const restrict arena, const size_t size);
void arena_destroy(arena_t *const restrict arena);

#ifdef __cplusplus
}
#endif
#endif
