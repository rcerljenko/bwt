#ifndef BWT_H
#define BWT_H

#ifdef _WIN32
	#define CALL_CONV __cdecl

	#ifdef BWT_STATIC
		#define DLL_EXPIMP
	#elif defined(BWT_DLL)
		#define DLL_EXPIMP __declspec(dllexport)
	#else
		#define DLL_EXPIMP __declspec(dllimport)
	#endif
#else
	#define CALL_CONV
	#define DLL_EXPIMP
#endif

typedef unsigned int bwt_size_t;

#ifdef __cplusplus
extern "C" {
#endif

DLL_EXPIMP bwt_size_t CALL_CONV bwt(void *const void_data, const bwt_size_t n);
DLL_EXPIMP void CALL_CONV ibwt(void *const void_data, const bwt_size_t n, bwt_size_t index);
DLL_EXPIMP bwt_size_t CALL_CONV rle(void *const void_data, const bwt_size_t n);
DLL_EXPIMP bwt_size_t CALL_CONV rld(void *const void_data, const bwt_size_t n);

#ifdef __cplusplus
}
#endif
#endif
