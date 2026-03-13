#include <string.h>
#include <limits.h>

#include "../include/internal.h"
#include "mtf.h"

DLL_EXPIMP void CALL_CONV mtf(void *const void_data, bwt_size_t n)
{
	if (!void_data || n < 2) {
		return;
	}

	unsigned char *restrict data = void_data;
	unsigned char i = UCHAR_MAX, list[UCHAR_MAX + 1];
	const unsigned char *pos;

	do {
		list[i] = i;
	} while (i--);

	while (n--) {
		pos = memchr(list, *data, sizeof(list));

		i = *pos;
		*data++ = pos - list;

		memmove(list + 1, list, pos - list);
		list[0] = i;
	}
}

DLL_EXPIMP void CALL_CONV mtfi(void *const void_data, bwt_size_t n)
{
	if (!void_data || n < 2) {
		return;
	}

	unsigned char *restrict data = void_data;
	unsigned char i = UCHAR_MAX, list[UCHAR_MAX + 1];

	do {
		list[i] = i;
	} while (i--);

	while (n--) {
		i = *data;
		*data = list[i];

		memmove(list + 1, list, i);
		list[0] = *data++;
	}
}
