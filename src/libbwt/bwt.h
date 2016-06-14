#ifndef BWT_H
#define BWT_H

#ifndef _WIN32
#define DLL_EXPINP
#define CALL_CONV
#else
#define CALL_CONV __cdecl
#ifdef BWT_DLL
#define DLL_EXPINP __declspec(dllexport)
#else
#define DLL_EXPINP __declspec(dllimport)
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned int bwt_size_t;

DLL_EXPINP bwt_size_t CALL_CONV bwt(unsigned char* __restrict data, const bwt_size_t n);
DLL_EXPINP void CALL_CONV ibwt(unsigned char* const __restrict data, const bwt_size_t n, bwt_size_t index);
DLL_EXPINP bwt_size_t CALL_CONV rle(unsigned char* __restrict data, const bwt_size_t n);
DLL_EXPINP bwt_size_t CALL_CONV rld(unsigned char* __restrict data, const bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif