#include <stdlib.h>

#include "../include/internal.h"
#include "utils.h"

#ifdef __SANITIZE_ADDRESS__

	#ifdef __cplusplus
extern "C" {
	#endif

const char *__asan_default_options(void);

const char *__asan_default_options(void)
{
	return "alloc_dealloc_mismatch=1:check_initialization_order=1:continue_on_error=2:detect_invalid_pointer_pairs=2:strict_string_checks=1";
}

	#ifdef __cplusplus
}
	#endif

#endif

DLL_EXPIMP arena_t CALL_CONV bwt_arena_create(const size_t size, const char bwt_mode)
{
	const size_t n = bwt_mode == BWT_MODE_COMPRESS ? size * 2 - 1 : size;

	return arena_create(sizeof(bwt_size_t) * size + n);
}

DLL_EXPIMP void CALL_CONV bwt_arena_destroy(arena_t *const restrict arena)
{
	arena_destroy(arena);
}
