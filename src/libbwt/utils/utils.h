#ifndef BWT_UTILS_H
#define BWT_UTILS_H

#include "../include/main.h"

#define BWT_MODE_COMPRESS 1U
#define BWT_MODE_DECOMPRESS 2U

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP arena_t CALL_CONV bwt_arena_create(const size_t size, const char bwt_mode);
DLL_EXPIMP void CALL_CONV bwt_arena_destroy(arena_t *const restrict arena);

#ifdef __cplusplus
}
#endif
#endif
