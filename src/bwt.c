#include "bwt.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#define OUTPUT_FLAG 'o'
#define DEC_FLAG 'd'
#define STDOUT_FLAG 'c'
#define REMOVE_FLAG 'r'
#define JOBS_FLAG 'j'
#define VERBOSE_FLAG 'v'
#define HELP_FLAG 'h'

#define ARGS "hdcrvj:o:"
#define FILE_EXT ".bwt"
#define BLOCK_SIZE 65536U
#define SIGTYPE SIGUSR1

#define FOPEN_INPUT_MODE "rb"
#define FOPEN_OUTPUT_MODE "wb"


struct header_t
{
	bwt_size_t block_size, index;
};

struct bwt_data_t
{
	unsigned char *data, status : 1;
	struct header_t header;
};

struct flags_t
{
	unsigned char dec : 1, remove : 1, verbose : 1;
};

struct stats_t
{
	size_t filesize_in, curr_fs_in, curr_fs_out;
	time_t start_time, end_time;
};

static char *filename;
static struct stats_t stats = {0};


static void *threaded_compress(void *void_bwt_data);
static void *threaded_decompress(void *void_bwt_data);
static int bwt_compress(FILE* restrict fp_in, FILE* restrict fp_out, const unsigned short thread_count);
static int bwt_decompress(FILE* restrict fp_in, FILE* restrict fp_out, const unsigned short thread_count);
static size_t get_filesize(FILE* restrict fp);
static size_t get_memusage();
static void show_statistics(const unsigned char signal);
static void sighandler();
static void show_help();


static void *threaded_compress(void *void_bwt_data)
{
	struct bwt_data_t *bwt_data = (struct bwt_data_t *)void_bwt_data;
	const bwt_size_t tmp_block_size = bwt_data->header.block_size;

	bwt_data->header.index = bwt(bwt_data->data, bwt_data->header.block_size);
	bwt_data->header.block_size = rle(bwt_data->data, bwt_data->header.block_size);

	if(bwt_data->header.block_size) bwt_data->status = 1;
	else
	{
		bwt_data->header.block_size = tmp_block_size;
		bwt_data->status = 0;
	}

	pthread_exit(NULL);
}

static void *threaded_decompress(void *void_bwt_data)
{
	struct bwt_data_t *bwt_data = (struct bwt_data_t *)void_bwt_data;

	if(bwt_data->status) bwt_data->header.block_size = rld(bwt_data->data, bwt_data->header.block_size);
	ibwt(bwt_data->data, bwt_data->header.block_size, bwt_data->header.index);

	pthread_exit(NULL);
}

static int bwt_compress(FILE* restrict fp_in, FILE* restrict fp_out, const unsigned short thread_count)
{
	const size_t fread_size = BLOCK_SIZE * thread_count;
	unsigned char *data = malloc(sizeof(unsigned char) * fread_size + 1);
	if(!data)
	{
		fprintf(stderr, "%s: Not enough memory.\n", filename);
		return EXIT_FAILURE;
	}

	bwt_size_t tmp_block_size;
	size_t i, n;
	unsigned short j;
	struct bwt_data_t bwt_data[thread_count];
	pthread_t threads[thread_count];

	const unsigned char main_block_size = (unsigned char) log2(BLOCK_SIZE);
	stats.curr_fs_out = fwrite(&main_block_size, sizeof(unsigned char), 1, fp_out);

	while((n = fread(data, sizeof(unsigned char), fread_size, fp_in)))
	{
		stats.curr_fs_in += n;

		for(i = j = 0; i < n; i += BLOCK_SIZE, j++)
		{
			bwt_data[j].data = data + i;

			if(i + BLOCK_SIZE <= n) bwt_data[j].header.block_size = BLOCK_SIZE;
			else bwt_data[j].header.block_size = n - i;

			pthread_create(&threads[j], NULL, threaded_compress, &bwt_data[j]);
		}

		for(i = 0; i < j; i++)
		{
			pthread_join(threads[i], NULL);

			tmp_block_size = bwt_data[i].header.block_size;
			if(!bwt_data[i].status) bwt_data[i].header.block_size = 0;

			fwrite(&bwt_data[i].header, sizeof(struct header_t), 1, fp_out);
			n = fwrite(bwt_data[i].data, sizeof(unsigned char), tmp_block_size, fp_out);

			if(n == tmp_block_size) stats.curr_fs_out += n + sizeof(struct header_t);
			else
			{
				perror(filename);
				free(data);
				return EXIT_FAILURE;
			}
		}
	}

	free(data);

	if(ferror(fp_in))
	{
		perror(filename);
		return EXIT_FAILURE;
	}
	else return EXIT_SUCCESS;
}

static int bwt_decompress(FILE* restrict fp_in, FILE* restrict fp_out, const unsigned short thread_count)
{
	bwt_size_t main_block_size;
	stats.curr_fs_in = fread(&main_block_size, sizeof(unsigned char), 1, fp_in);
	if(stats.curr_fs_in != 1) return EXIT_SUCCESS;

	main_block_size = 1U << main_block_size;
	unsigned char *data = malloc(sizeof(unsigned char) * main_block_size * thread_count + 1);
	if(!data)
	{
		fprintf(stderr, "%s: Not enough memory.\n", filename);
		return EXIT_FAILURE;
	}

	size_t status, n;
	unsigned short i = 0;
	struct bwt_data_t bwt_data[thread_count];
	pthread_t threads[thread_count];

	while(fread(&bwt_data[i].header, sizeof(struct header_t), 1, fp_in) == 1)
	{
		bwt_data[i].data = data + main_block_size * i;
		if(bwt_data[i].header.block_size)
		{
			status = fread(bwt_data[i].data, sizeof(unsigned char), bwt_data[i].header.block_size, fp_in);
			if(status == bwt_data[i].header.block_size)
			{
				stats.curr_fs_in += status;
				bwt_data[i].status = 1;
			}
			else
			{
				if(ferror(fp_in)) perror(filename);
				else fprintf(stderr, "%s: Invalid input file format.\n", filename);

				free(data);
				return EXIT_FAILURE;
			}
		}
		else
		{
			bwt_data[i].header.block_size = fread(bwt_data[i].data, sizeof(unsigned char), main_block_size, fp_in);
			if(ferror(fp_in))
			{
				perror(filename);
				free(data);
				return EXIT_FAILURE;
			}
			else
			{
				stats.curr_fs_in += bwt_data[i].header.block_size;
				bwt_data[i].status = 0;
			}
		}

		pthread_create(&threads[i], NULL, threaded_decompress, &bwt_data[i]);

		stats.curr_fs_in += sizeof(struct header_t);
		i++;

		if(i == thread_count)
		{
			for(i = n = 0; i < thread_count; i++)
			{
				pthread_join(threads[i], NULL);
				n += bwt_data[i].header.block_size;
			}

			status = fwrite(data, sizeof(unsigned char), n, fp_out);
			if(status == n)
			{
				stats.curr_fs_out += n;
				i = 0;
			}
			else
			{
				perror(filename);
				free(data);
				return EXIT_FAILURE;
			}
		}
	}

	if(i)
	{
		unsigned short j;
		for(j = n = 0; j < i; j++)
		{
			pthread_join(threads[j], NULL);
			n += bwt_data[j].header.block_size;
		}

		status = fwrite(data, sizeof(unsigned char), n, fp_out);
		if(status == n) stats.curr_fs_out += n;
		else
		{
			perror(filename);
			free(data);
			return EXIT_FAILURE;
		}
	}

	free(data);
	return EXIT_SUCCESS;
}

static size_t get_filesize(FILE* restrict fp)
{
	fseek(fp, 0, SEEK_END);
	const size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return size;
}

static size_t get_memusage()
{
	FILE* restrict fp = fopen("/proc/self/statm", "rb");
	if(!fp) return 0;

	unsigned int vm_rss;
	const unsigned short page_size = sysconf(_SC_PAGESIZE);

	if(fscanf(fp, "%*u %u", &vm_rss) != 1) vm_rss = 0;
	fclose(fp);

	return vm_rss * page_size;
}

static void show_statistics(const unsigned char signal)
{
	char time_buffer[8];
	float diff_perc = 0, ratio = 0, speed = 0;

	const size_t diff_fs = labs(stats.curr_fs_in - stats.curr_fs_out);
	const time_t diff_time = difftime(stats.end_time, stats.start_time);
	if(diff_time) speed = (float)stats.curr_fs_in / (1024 * 1024 * diff_time);

	if(stats.curr_fs_in && stats.curr_fs_out)
	{
		if(stats.curr_fs_in > stats.curr_fs_out) diff_perc = (100 * diff_fs) / (float)stats.curr_fs_in;
		else diff_perc = (100 * diff_fs) / (float)stats.curr_fs_out;

		ratio = stats.curr_fs_in / (float)stats.curr_fs_out;
	}

	const struct tm* restrict time_info = localtime(&diff_time);
	strftime(time_buffer, sizeof(time_buffer), "%Mm:%Ss", time_info);

	if(signal && stats.filesize_in)
	{
		const unsigned short progress = (100 * stats.curr_fs_in) / stats.filesize_in;
		fprintf(stderr, "Progress: %hu%%\n", progress);
	}

	fprintf(stderr, "Bytes read: %lu B\n"
	"Bytes written: %lu B\n"
	"Diff: %lu B (%.2f%%)\n"
	"Ratio: %.2f\n"
	"Time: %s\n"
	"Speed: %.2f MB/s\n"
	, stats.curr_fs_in, stats.curr_fs_out, diff_fs, diff_perc, ratio, time_buffer, speed);

	if(signal)
	{
		const float memory = (float) get_memusage() / (1024 * 1024);
		fprintf(stderr, "Memory (RAM): %.2f MB\n\n", memory);
	}
}

static void sighandler()
{
	time(&stats.end_time);
	show_statistics(1);
}

static void show_help()
{
	fprintf(stderr, "Usage: %s [<input_file>] [OPTIONS]\n"
	"\tBuild date: " __DATE__ " @ " __TIME__ "\n\n"
	"\tIf <input_file> is omitted, input file is STDIN.\n"
	"\tIf <input_file> is STDIN and output file is omitted (no valid -%c flag), output file is STDOUT (like -%c flag).\n"
	"\tOther processes can send signal SIGUSR1 to get progress info (unavailable when input file is STDIN).\n\n"
	"Possible options (can be combined together):\n"
	"\t-%c - Write output to STDOUT (ignored if valid -%c flag exists).\n"
	"\t-%c - Decompression mode.\n"
	"\t-%c - Show help and exit.\n"
	"\t-%c<jobs_count> - Number of parallel jobs (threads). If less than 1 or greater than available threads, fallback to available threads.\n"
	"\t-%c <output_file> - Custom output filename (if omitted, output filename is input_file" FILE_EXT ").\n"
	"\t-%c - Remove input file after successful operation (ignored when input file is STDIN).\n"
	"\t-%c - Verbose mode - show statistics after successful operation.\n"
	, filename, OUTPUT_FLAG, STDOUT_FLAG, STDOUT_FLAG, OUTPUT_FLAG, DEC_FLAG, HELP_FLAG, JOBS_FLAG, OUTPUT_FLAG, REMOVE_FLAG, VERBOSE_FLAG);
}

int main(const int argc, char **argv)
{
	int c;
	unsigned long jobs = 0;
	char *input, output[PATH_MAX] = {0};
	FILE *fp_in = stdin, *fp_out = NULL;
	struct flags_t flags = {0};

	filename = basename(argv[0]);

	while((c = getopt(argc, argv, ARGS)) != -1)
	{
		switch(c)
		{
			case HELP_FLAG:
			{
				show_help();
				return EXIT_SUCCESS;
			}
			case DEC_FLAG:
			{
				flags.dec = 1;
				break;
			}
			case REMOVE_FLAG:
			{
				flags.remove = 1;
				break;
			}
			case VERBOSE_FLAG:
			{
				flags.verbose = 1;
				break;
			}
			case JOBS_FLAG:
			{
				jobs = strtoul(optarg, NULL, 10);
				break;
			}
			case STDOUT_FLAG:
			{
				fp_out = stdout;
				break;
			}
			case OUTPUT_FLAG:
			{
				strcpy(output, optarg);
				break;
			}
			default:
			{
				show_help();
				return EXIT_FAILURE;
			}
		}
	}

	input = argv[optind];

	if(!fp_out && !output[0])
	{
		if(input)
		{
			const unsigned char ext_len = strlen(FILE_EXT);
			strncpy(output, basename(input), NAME_MAX - ext_len);

			if(flags.dec)
			{
				char *ext_pos = strstr(output + strlen(output) - ext_len, FILE_EXT);
				if(ext_pos) *ext_pos = 0;
			}
			else strcat(output, FILE_EXT);
		}
		else fp_out = stdout;
	}

	if(input && output[0] && strcmp(input, output) == 0)
	{
		fprintf(stderr, "%s: Input and output file must be different.\n", filename);
		return EXIT_FAILURE;
	}

	if(input)
	{
		fp_in = fopen(input, FOPEN_INPUT_MODE);
		if(!fp_in)
		{
			perror(filename);
			return EXIT_FAILURE;
		}

		stats.filesize_in = get_filesize(fp_in);
		if(!stats.filesize_in)
		{
			fclose(fp_in);
			return EXIT_SUCCESS;
		}
	}
	else flags.remove = 0;

	if(output[0])
	{
		fp_out = fopen(output, FOPEN_OUTPUT_MODE);
		if(!fp_out)
		{
			perror(filename);
			fclose(fp_in);
			return EXIT_FAILURE;
		}
	}

	int status;
	unsigned short thread_count = sysconf(_SC_NPROCESSORS_ONLN);
	if(jobs && jobs < thread_count) thread_count = jobs;

	signal(SIGTYPE, sighandler);
	time(&stats.start_time);

	if(!flags.dec) status = bwt_compress(fp_in, fp_out, thread_count);
	else status = bwt_decompress(fp_in, fp_out, thread_count);

	time(&stats.end_time);
	fclose(fp_in);
	fclose(fp_out);

	if(status == EXIT_SUCCESS)
	{
		if(flags.verbose) show_statistics(0);
		if(flags.remove) remove(input);
	}
	else if(output[0]) remove(output);

	return status;
}