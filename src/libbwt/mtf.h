#ifndef MTF_H
#define MTF_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP void CALL_CONV mtf(void *const void_data, bwt_size_t n);
DLL_EXPIMP void CALL_CONV mtfi(void *const void_data, bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif
