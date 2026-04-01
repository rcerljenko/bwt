#ifndef BWT_INTERNAL_H
#define BWT_INTERNAL_H

#ifdef _WIN32
	#define BWT_DLL
#endif

#ifdef __SANITIZE_ADDRESS__
	#ifdef __cplusplus
extern "C" {
	#endif

static const char *__asan_default_options();

static const char *__asan_default_options()
{
	return "alloc_dealloc_mismatch=1:check_initialization_order=1:continue_on_error=2:strict_string_checks=1";
}

	#ifdef __cplusplus
}
	#endif
#endif

#endif
