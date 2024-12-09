#ifndef BWT_H
#define BWT_H

#ifndef _WIN32
	#define DLL_EXPIMP
	#define CALL_CONV
#else
	#define CALL_CONV __cdecl
	#ifdef BWT_DLL
		#define DLL_EXPIMP __declspec(dllexport)
	#else
		#define DLL_EXPIMP __declspec(dllimport)
	#endif
#endif

typedef unsigned int bwt_size_t;

#ifdef __cplusplus
extern "C"
{
#endif

DLL_EXPIMP bwt_size_t CALL_CONV bwt(void* const void_data, const bwt_size_t n);
DLL_EXPIMP void CALL_CONV ibwt(void* const void_data, const bwt_size_t n, bwt_size_t index);
DLL_EXPIMP bwt_size_t CALL_CONV rle(void* const void_data, const bwt_size_t n);
DLL_EXPIMP bwt_size_t CALL_CONV rld(void* const void_data, const bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif
