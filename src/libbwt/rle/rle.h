#ifndef RLE_H
#define RLE_H

#include "../include/main.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP bwt_size_t CALL_CONV rle(void *const void_data, const bwt_size_t n, const arena_t *const restrict arena);
DLL_EXPIMP bwt_size_t CALL_CONV rld(void *const void_data, const bwt_size_t n, const arena_t *const restrict arena);

#ifdef __cplusplus
}
#endif
#endif
