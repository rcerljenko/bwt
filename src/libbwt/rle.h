#ifndef RLE_H
#define RLE_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP bwt_size_t CALL_CONV rle(void *const void_data, const bwt_size_t n);
DLL_EXPIMP bwt_size_t CALL_CONV rld(void *const void_data, const bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif
