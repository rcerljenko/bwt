#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SANITIZE_ADDRESS__
const char *__asan_default_options(void);

const char *__asan_default_options(void)
{
	return "alloc_dealloc_mismatch=1:check_initialization_order=1:continue_on_error=2:strict_string_checks=1";
}
#endif

#ifdef __cplusplus
}
#endif
