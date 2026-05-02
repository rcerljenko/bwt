#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "../include/internal.h"
#include "rle.h"

DLL_EXPIMP bwt_size_t rle(void *const void_data, const bwt_size_t n)
{
	if (!void_data || n < 4) {
		return 0;
	}

	unsigned char *restrict result = malloc(n);

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

	result -= len;

	if (len < n) {
		memcpy(data - n, result, len);
	} else {
		len = 0;
	}

	free(result);

	return len;
}

DLL_EXPIMP bwt_size_t rld(void *const void_data, const bwt_size_t n)
{
	if (!void_data || n < 3) {
		return 0;
	}

	unsigned char *restrict tmp_data = malloc(n);

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

	free(tmp_data - n);

	return data - start;
}
