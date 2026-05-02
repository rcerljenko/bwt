#ifndef MTF_H
#define MTF_H

#include "../include/main.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP void mtf(void *const void_data, bwt_size_t n);
DLL_EXPIMP void mtfi(void *const void_data, bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif
