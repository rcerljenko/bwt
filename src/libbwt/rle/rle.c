#include <string.h>
#include <limits.h>

#include "../include/internal.h"
#include "rle.h"

DLL_EXPIMP bwt_size_t CALL_CONV rle(void *const void_data, const bwt_size_t n, const arena_t *const restrict arena)
{
	if (!void_data || n < 4) {
		return 0;
	}

	unsigned char *restrict result = arena_allocate(arena, n);

	if (!result) {
		return 0;
	}

	unsigned char *restrict data = void_data;
	bwt_size_t len = 0;
	unsigned short count;
	unsigned char curr_char;
	const unsigned char *const end = data + n;

	while (data < end && len < n) {
		curr_char = *data++;

		for (count = 0; data < end && curr_char == *data && count <= UCHAR_MAX; count++, data++);

		*result++ = curr_char;

		if (count) {
			*result++ = curr_char;
			*result++ = count - 1;
			len += 3;
		} else {
			len++;
		}
	}

	if (len < n) {
		memcpy(data - n, result - len, len);
	} else {
		len = 0;
	}

	arena_free(arena, n);

	return len;
}

DLL_EXPIMP bwt_size_t CALL_CONV rld(void *const void_data, const bwt_size_t n, const arena_t *const restrict arena)
{
	if (!void_data || n < 3) {
		return 0;
	}

	unsigned char *restrict tmp_data = arena_allocate(arena, n);

	if (!tmp_data) {
		return 0;
	}

	unsigned char *restrict data = void_data;
	unsigned char curr_char;
	const unsigned char *const start = data;
	const unsigned char *const tmp_end = tmp_data + n - 1;

	memcpy(tmp_data, data, n);

	while (tmp_data <= tmp_end) {
		*data++ = curr_char = *tmp_data++;

		if (tmp_data < tmp_end && curr_char == *tmp_data) {
			tmp_data++;

			do {
				*data++ = curr_char;
			} while ((*tmp_data)--);

			tmp_data++;
		}
	}

	arena_free(arena, n);

	return data - start;
}
