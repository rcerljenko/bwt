#ifndef BWT_H
#define BWT_H

#ifndef _WIN32
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <limits.h>


typedef unsigned int bwt_size_t;

struct bwt_info_t
{
	unsigned char* __restrict rotations;
	bwt_size_t len;
};


#ifdef _WIN32
static inline int bwt_cmp(void* const arg, const void* const a, const void* const b);
#else
static inline int bwt_cmp(const void* const a, const void* const b, void* const arg);
#endif
static inline int ibwt_cmp(const void* const a, const void* const b);
static bwt_size_t bwt(unsigned char* __restrict data, const bwt_size_t n);
static void ibwt(unsigned char* const __restrict data, const bwt_size_t n, bwt_size_t index);
static bwt_size_t rle(unsigned char* __restrict data, const bwt_size_t n);
static bwt_size_t rld(unsigned char* __restrict data, const bwt_size_t n);


#ifdef _WIN32
static inline int bwt_cmp(void* const arg, const void* const a, const void* const b)
#else
static inline int bwt_cmp(const void* const a, const void* const b, void* const arg)
#endif
{
	const bwt_size_t i = *(bwt_size_t *)a;
	const bwt_size_t j = *(bwt_size_t *)b;
	const struct bwt_info_t* const __restrict data_info = (struct bwt_info_t *)arg;

	return memcmp(data_info->rotations + i, data_info->rotations + j, data_info->len);
}

static inline int ibwt_cmp(const void* const a, const void* const b)
{
	return *(unsigned char *)a - *(unsigned char *)b;
}

static bwt_size_t bwt(unsigned char* __restrict data, const bwt_size_t n)
{
	if(n < 2) return 0;

	bwt_size_t i, index = n;
	struct bwt_info_t data_info;
	data_info.rotations = malloc(sizeof(unsigned char) * n * 2 + 1);
	bwt_size_t* __restrict positions = malloc(sizeof(bwt_size_t) * n);

	memcpy(data_info.rotations, data, n);
	memcpy(data_info.rotations + n, data, n);
	data_info.len = n;

	positions += n;
	while((*(--positions) = --index));

#ifdef _WIN32
	qsort_s(positions, n, sizeof(bwt_size_t), bwt_cmp, &data_info);
#else
	qsort_r(positions, n, sizeof(bwt_size_t), bwt_cmp, &data_info);
#endif

	for(i = 0; i < n; i++)
	{
		*data++ = data_info.rotations[(*positions + n - 1) % n];
		if(!*positions++) index = i;
	}

	free(positions - n);
	free(data_info.rotations);

	return index;
}

static void ibwt(unsigned char* const __restrict data, const bwt_size_t n, bwt_size_t index)
{
	if(n < 2) return;

	bwt_size_t count, pos_cache[UCHAR_MAX + 1] = {0};
	unsigned char* const __restrict result = malloc(sizeof(unsigned char) * n * 2 + 1);
	unsigned char* const __restrict sorted = result + n;
	unsigned char *pos, *curr_pos = sorted;

	memcpy(sorted, data, n);
	qsort(sorted, n, sizeof(unsigned char), ibwt_cmp);

	while(curr_pos-- > result)
	{
		*curr_pos = data[index];
		for(count = 0, pos = data; (pos = memchr(pos, *curr_pos, index - (pos - data))); count++, pos++);

		if(!pos_cache[*curr_pos]) pos_cache[*curr_pos] = ((unsigned char *) memchr(sorted, *curr_pos, n)) - sorted + 1;
		index = pos_cache[*curr_pos] + count - 1;
	}

	memcpy(data, result, n);
	free(result);
}

static bwt_size_t rle(unsigned char* __restrict data, const bwt_size_t n)
{
	if(n < 4) return 0;

	bwt_size_t len = 0;
	unsigned short count;
	unsigned char curr_char;
	unsigned char* const end = data + n;
	unsigned char* __restrict result = malloc(sizeof(unsigned char) * n + 1);

	while(data < end && len < n)
	{
		curr_char = *data++;
		for(count = 0; data < end && curr_char == *data && count <= UCHAR_MAX; count++, data++);

		*result++ = curr_char;
		if(count)
		{
			*result++ = curr_char;
			*result++ = (unsigned char)(count - 1);
			len += 3;
		}
		else len++;
	}

	result -= len;
	if(len < n) memcpy(data - n, result, len);
	else len = 0;

	free(result);
	return len;
}

static bwt_size_t rld(unsigned char* __restrict data, const bwt_size_t n)
{
	if(n < 3) return n;

	unsigned char curr_char;
	unsigned char* __restrict tmp_data = malloc(sizeof(unsigned char) * n + 1);
	unsigned char* const start = data;
	unsigned char* const tmp_end = tmp_data + n - 1;

	memcpy(tmp_data, data, n);

	while(tmp_data <= tmp_end)
	{
		*data++ = curr_char = *tmp_data++;

		if(tmp_data < tmp_end && curr_char == *tmp_data)
		{
			tmp_data++;
			do
			{
				*data++ = curr_char;
			}
			while((*tmp_data)--);
			tmp_data++;
		}
	}

	free(tmp_data - n);
	return data - start;
}

#endif