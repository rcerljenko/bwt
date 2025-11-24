#ifdef __linux__
	#define _GNU_SOURCE
#elif defined(_WIN32)
	#define BWT_DLL
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "bwt.h"

struct bwt_info_t {
		unsigned char *restrict rotations;
		bwt_size_t len;
};

#ifdef __linux__
static inline int bwt_cmp(const void *const a, const void *const b, void *const arg);
#elif defined(_WIN32) || defined(__APPLE__)
static inline int bwt_cmp(void *const arg, const void *const a, const void *const b);
#endif
static inline int ibwt_cmp(const void *const a, const void *const b);

#ifdef __linux__
static inline int bwt_cmp(const void *const a, const void *const b, void *const arg)
#elif defined(_WIN32) || defined(__APPLE__)
static inline int bwt_cmp(void *const arg, const void *const a, const void *const b)
#endif
{
	const bwt_size_t i = *(bwt_size_t *) a;
	const bwt_size_t j = *(bwt_size_t *) b;
	const struct bwt_info_t *const restrict data_info = arg;

	return memcmp(data_info->rotations + i, data_info->rotations + j, data_info->len);
}

static inline int ibwt_cmp(const void *const a, const void *const b)
{
	return *(unsigned char *) a - *(unsigned char *) b;
}

DLL_EXPIMP bwt_size_t CALL_CONV bwt(void *const void_data, const bwt_size_t n)
{
	if (!void_data || n < 2) {
		return 0;
	}

	unsigned char *restrict data = void_data;
	bwt_size_t i, index = n;
	struct bwt_info_t data_info;
	data_info.rotations = malloc(n * 2 - 1);
	bwt_size_t *restrict positions = malloc(sizeof(bwt_size_t) * n);

	memcpy(data_info.rotations, data, n);
	memcpy(data_info.rotations + n, data, n - 1);
	data_info.len = n;

	positions += n;
	while ((*(--positions) = --index));

#ifdef __linux__
	qsort_r(positions, n, sizeof(bwt_size_t), bwt_cmp, &data_info);
#elif defined(__APPLE__)
	qsort_r(positions, n, sizeof(bwt_size_t), &data_info, bwt_cmp);
#elif defined(_WIN32)
	qsort_s(positions, n, sizeof(bwt_size_t), bwt_cmp, &data_info);
#endif

	for (i = 0; i < n; i++) {
		*data++ = data_info.rotations[(*positions + n - 1) % n];

		if (!*positions++) {
			index = i;
		}
	}

	free(positions - n);
	free(data_info.rotations);

	return index;
}

DLL_EXPIMP void CALL_CONV ibwt(void *const void_data, const bwt_size_t n, bwt_size_t index)
{
	if (!void_data || n < 2 || index >= n) {
		return;
	}

	unsigned char *restrict data = void_data;
	bwt_size_t i;
	unsigned char curr_char;
	const unsigned char *pos;
	unsigned char *const restrict tmp_data = malloc(n);
	bwt_size_t *restrict transform = malloc(sizeof(bwt_size_t) * n);

	memcpy(tmp_data, data, n);
	qsort(data, n, 1, ibwt_cmp);

	for (i = 0, curr_char = *data, pos = tmp_data; i < n; i++, data++, pos++) {
		if (curr_char != *data) {
			curr_char = *data;
			pos = tmp_data;
		}

		pos = memchr(pos, curr_char, n - (pos - tmp_data));
		*transform++ = pos - tmp_data;
	}

	data -= n;
	transform -= n;

	while (i--) {
		index = transform[index];
		*data++ = tmp_data[index];
	}

	free(transform);
	free(tmp_data);
}

DLL_EXPIMP bwt_size_t CALL_CONV rle(void *const void_data, const bwt_size_t n)
{
	if (!void_data || n < 4) {
		return 0;
	}

	unsigned char *restrict data = void_data;
	bwt_size_t len = 0;
	unsigned short count;
	unsigned char curr_char;
	const unsigned char *const end = data + n;
	unsigned char *restrict result = malloc(n);

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

DLL_EXPIMP bwt_size_t CALL_CONV rld(void *const void_data, const bwt_size_t n)
{
	if (!void_data || n < 3) {
		return 0;
	}

	unsigned char *restrict data = void_data;
	unsigned char curr_char;
	unsigned char *restrict tmp_data = malloc(n);
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
