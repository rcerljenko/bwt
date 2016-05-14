#ifndef BWT_H
#define BWT_H

#ifndef _WIN32
#define __cdecl
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned int bwt_size_t;

bwt_size_t __cdecl bwt(unsigned char* __restrict data, const bwt_size_t n);
void __cdecl ibwt(unsigned char* const __restrict data, const bwt_size_t n, bwt_size_t index);
bwt_size_t __cdecl rle(unsigned char* __restrict data, const bwt_size_t n);
bwt_size_t __cdecl rld(unsigned char* __restrict data, const bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif