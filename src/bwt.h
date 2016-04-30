#ifndef BWT_H
#define BWT_H

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <limits.h>


typedef unsigned int bwt_size_t;

struct bwt_info_t
{
	unsigned char* restrict rotations;
	bwt_size_t len;
};


static inline int bwt_cmp(const void *a, const void *b, void *arg);
static inline int ibwt_cmp(const void *a, const void *b);
static bwt_size_t bwt(unsigned char* restrict data, const bwt_size_t n);
static void ibwt(unsigned char* restrict data, const bwt_size_t n, bwt_size_t index);
static bwt_size_t rle(unsigned char* restrict data, const bwt_size_t n);
static bwt_size_t rld(unsigned char* restrict data, const bwt_size_t n);


static inline int bwt_cmp(const void *a, const void *b, void *arg)
{
	const bwt_size_t i = *(bwt_size_t *)a;
	const bwt_size_t j = *(bwt_size_t *)b;
	const struct bwt_info_t* restrict data_info = (struct bwt_info_t *)arg;

	return memcmp(data_info->rotations + i, data_info->rotations + j, data_info->len);
}

static inline int ibwt_cmp(const void *a, const void *b)
{
	return *(unsigned char *)a - *(unsigned char *)b;
}

static bwt_size_t bwt(unsigned char* restrict data, const bwt_size_t n)
{
	bwt_size_t i, index;
	bwt_size_t* restrict positions = malloc(sizeof(bwt_size_t) * n);
	struct bwt_info_t data_info;
	data_info.rotations = malloc(sizeof(unsigned char) * n * 2 + 1);

	memcpy(data_info.rotations, data, n);
	memcpy(data_info.rotations + n, data, n);
	data_info.len = n;

	for(i = 0; i < n; i++) positions[i] = i;
	qsort_r(positions, n, sizeof(bwt_size_t), bwt_cmp, &data_info);

	for(i = 0; i < n; i++)
	{
		data[i] = data_info.rotations[(positions[i] + n - 1) % n];
		if(!positions[i]) index = i;
	}

	free(positions);
	free(data_info.rotations);

	return index;
}

static void ibwt(unsigned char* restrict data, const bwt_size_t n, bwt_size_t index)
{
	bwt_size_t i, count;
	bwt_size_t pos_cache[UCHAR_MAX + 1] = {0};
	unsigned char* restrict pos;
	unsigned char* restrict result = malloc(sizeof(unsigned char) * n * 2 + 1);
	unsigned char* restrict sorted = result + n;

	memcpy(sorted, data, n);
	qsort(sorted, n, sizeof(unsigned char), ibwt_cmp);

	for(i = 0; i < n; i++)
	{
		result[n - i - 1] = data[index];

		for(count = 0, pos = data; (pos = memchr(pos, data[index], index - (pos - data))); count++, pos++);

		if(!pos_cache[data[index]]) pos_cache[data[index]] = ((unsigned char *) memchr(sorted, data[index], n)) - sorted + 1;
		index = pos_cache[data[index]] + count - 1;
	}

	memcpy(data, result, n);
	free(result);
}

static bwt_size_t rle(unsigned char* restrict data, const bwt_size_t n)
{
	bwt_size_t i, j, len;
	unsigned short count;
	unsigned char* restrict result = malloc(sizeof(unsigned char) * n + 1);

	for(i = len = 0; i < n && len < n; i = j)
	{
		for(j = i + 1, count = 0; j < n && data[i] == data[j] && count <= UCHAR_MAX; j++, count++);

		result[len++] = data[i];
		if(count)
		{
			result[len++] = data[i];
			result[len++] = (unsigned char)(count - 1);
		}
	}

	if(len < n) memcpy(data, result, len);
	else len = 0;

	free(result);
	return len;
}

static bwt_size_t rld(unsigned char* restrict data, const bwt_size_t n)
{
	bwt_size_t i = 0, len = 0;
	unsigned short j;
	unsigned char* restrict tmp_data = malloc(sizeof(unsigned char) * n + 1);

	memcpy(tmp_data, data, n);

	while(i < n)
	{
		data[len++] = tmp_data[i];

		if(i < n - 2 && tmp_data[i] == tmp_data[i + 1])
		{
			for(j = 0; j <= tmp_data[i + 2]; j++) data[len++] = tmp_data[i];
			i += 3;
		}
		else i++;
	}

	free(tmp_data);
	return len;
}

#endif