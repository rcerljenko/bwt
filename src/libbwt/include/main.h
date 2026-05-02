#ifndef BWT_MAIN_H
#define BWT_MAIN_H

#ifdef _WIN32
	#ifdef BWT_STATIC
		#define DLL_EXPIMP
	#elif defined(BWT_DLL)
		#define DLL_EXPIMP __declspec(dllexport)
	#else
		#define DLL_EXPIMP __declspec(dllimport)
	#endif
#else
	#define DLL_EXPIMP
#endif

typedef unsigned int bwt_size_t;

#endif
