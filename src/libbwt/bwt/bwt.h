#ifndef BWT_H
#define BWT_H

#include "../include/main.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP bwt_size_t bwt(void *const void_data, const bwt_size_t n);
DLL_EXPIMP void bwti(void *const void_data, const bwt_size_t n, bwt_size_t index);

#ifdef __cplusplus
}
#endif
#endif
