#ifndef BWT_H
#define BWT_H

#include "../include/main.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP bwt_size_t CALL_CONV bwt(void *const void_data, const bwt_size_t n, const arena_t *const restrict arena);
DLL_EXPIMP void CALL_CONV bwti(void *const void_data, const bwt_size_t n, bwt_size_t index, const arena_t *const restrict arena);

#ifdef __cplusplus
}
#endif
#endif
