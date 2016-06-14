#ifndef BWT_H
#define BWT_H

#ifndef _WIN32
#define DLL_EXPINP
#define __cdecl
#elif defined(BWT_DLL)
#define DLL_EXPINP __declspec(dllexport)
#else
#define DLL_EXPINP __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned int bwt_size_t;

DLL_EXPINP bwt_size_t __cdecl bwt(unsigned char* __restrict data, const bwt_size_t n);
DLL_EXPINP void __cdecl ibwt(unsigned char* const __restrict data, const bwt_size_t n, bwt_size_t index);
DLL_EXPINP bwt_size_t __cdecl rle(unsigned char* __restrict data, const bwt_size_t n);
DLL_EXPINP bwt_size_t __cdecl rld(unsigned char* __restrict data, const bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif