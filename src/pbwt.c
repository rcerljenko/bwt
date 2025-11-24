#ifndef _WIN32
	#if defined(__linux__) && defined(__GLIBC__)
		#define _GNU_SOURCE
	#endif

	#include <signal.h>
	#include <limits.h>
	#include <libgen.h>
	#include <unistd.h>
	#include <pthread.h>

	#define MAX_PATH PATH_MAX
	#define SIGTYPE SIGUSR1
	#define THREAD_RETURN NULL

typedef pthread_t thread_t;
#else
	#include <windows.h>
	#include <process.h>
	#include <psapi.h>
	#include <fcntl.h>
	#include <io.h>

	#define THREAD_RETURN 0U

typedef HANDLE thread_t;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "libbwt/bwt.h"

#define OUTPUT_FLAG 'o'
#define DEC_FLAG 'd'
#define STDOUT_FLAG 'c'
#define REMOVE_FLAG 'r'
#define PRESET_FLAG 'p'
#define JOBS_FLAG 'j'
#define VERBOSE_FLAG 'v'
#define HELP_FLAG 'h'

#define ARGS "hdcrvp:j:o:"
#define FILE_EXT ".bwt"
#define PRESET_MIN 1U
#define PRESET_MAX 9U
#define PRESET_DEF 5U
#define SIZE_THRESH 11U

#define FOPEN_INPUT_MODE "rb"
#define FOPEN_OUTPUT_MODE "wb"

struct header_t {
		bwt_size_t block_size, index;
};

struct bwt_data_t {
		unsigned char *data, status : 1;
		struct header_t header;
};

struct flags_t {
		unsigned char dec : 1, remove : 1, verbose : 1;
};

struct stats_t {
		size_t filesize_in, curr_fs_in, curr_fs_out;
		time_t start_time, end_time;
};

#ifndef _WIN32
static char *filename;
#else
static char filename[_MAX_FNAME + 1];
static const char *optarg;
static unsigned short optind;
#endif
static struct stats_t stats = {0};

#ifndef _WIN32
static void *threaded_compress(void *const void_bwt_data);
static void *threaded_decompress(void *const void_bwt_data);
static void sighandler(const int signum);
#else
static unsigned int __stdcall threaded_compress(void *const void_bwt_data);
static unsigned int __stdcall threaded_decompress(void *const void_bwt_data);
static short getopt(const unsigned short argc, char **const restrict argv, const char *const restrict args);
#endif
static int bwt_compress(FILE *const restrict fp_in, FILE *const restrict fp_out, const unsigned short thread_count, const unsigned char block_size);
static int bwt_decompress(FILE *const restrict fp_in, FILE *const restrict fp_out, const unsigned short thread_count);
static void create_output_path(char *restrict input, char *const output, const unsigned char dec_flag);
static size_t get_filesize(FILE *const restrict fp);
static unsigned short get_threadcount(void);
static size_t get_memusage(void);
static void show_statistics(const int signum);
static void show_help(void);

#ifndef _WIN32
static void *threaded_compress(void *const void_bwt_data)
#else
static unsigned int __stdcall threaded_compress(void *const void_bwt_data)
#endif
{
	struct bwt_data_t *const restrict bwt_data = void_bwt_data;
	const bwt_size_t tmp_block_size = bwt_data->header.block_size;

	bwt_data->header.index = bwt(bwt_data->data, bwt_data->header.block_size);
	bwt_data->header.block_size = rle(bwt_data->data, bwt_data->header.block_size);

	if (bwt_data->header.block_size) {
		bwt_data->status = 1;
	} else {
		bwt_data->header.block_size = tmp_block_size;
		bwt_data->status = 0;
	}

	return THREAD_RETURN;
}

#ifndef _WIN32
static void *threaded_decompress(void *const void_bwt_data)
#else
static unsigned int __stdcall threaded_decompress(void *const void_bwt_data)
#endif
{
	struct bwt_data_t *const restrict bwt_data = void_bwt_data;

	if (bwt_data->status) {
		bwt_data->header.block_size = rld(bwt_data->data, bwt_data->header.block_size);
	}

	ibwt(bwt_data->data, bwt_data->header.block_size, bwt_data->header.index);

	return THREAD_RETURN;
}

static int bwt_compress(FILE *const restrict fp_in, FILE *const restrict fp_out, const unsigned short thread_count, const unsigned char block_size)
{
	const bwt_size_t main_block_size = 1U << block_size;
	const size_t fread_size = main_block_size * thread_count;

	unsigned char *const data = malloc(fread_size);

	if (!data) {
		fprintf(stderr, "%s: Not enough memory.\n", filename);

		return EXIT_FAILURE;
	}

	size_t i, n;
	unsigned short j;
	bwt_size_t tmp_block_size;
	unsigned char status = 1;

	struct bwt_data_t *const restrict bwt_data = malloc(sizeof(struct bwt_data_t) * thread_count);
	thread_t *const restrict threads = malloc(sizeof(thread_t) * thread_count);

	stats.curr_fs_out = fwrite(&block_size, 1, 1, fp_out);

	while ((n = fread(data, 1, fread_size, fp_in))) {
		stats.curr_fs_in += n;

		for (i = j = 0; i < n; i += main_block_size, j++) {
			bwt_data[j].data = data + i;

			if (i + main_block_size <= n) {
				bwt_data[j].header.block_size = main_block_size;
			} else {
				bwt_data[j].header.block_size = n - i;
			}

#ifndef _WIN32
			pthread_create(&threads[j], NULL, threaded_compress, &bwt_data[j]);
#else
			threads[j] = (thread_t) _beginthreadex(NULL, 0, threaded_compress, &bwt_data[j], 0, NULL);
#endif
		}

		for (i = 0; i < j; i++) {
#ifndef _WIN32
			pthread_join(threads[i], NULL);
#else
			WaitForSingleObject(threads[i], INFINITE);
			CloseHandle(threads[i]);
#endif
			tmp_block_size = bwt_data[i].header.block_size;

			if (!bwt_data[i].status) {
				bwt_data[i].header.block_size = 0;
			}

			fwrite(&bwt_data[i].header, sizeof(struct header_t), 1, fp_out);
			n = fwrite(bwt_data[i].data, 1, tmp_block_size, fp_out);

			if (n == tmp_block_size) {
				stats.curr_fs_out += n + sizeof(struct header_t);
			} else {
				perror(filename);
				status = 0;

				break;
			}
		}

		if (!status) {
			break;
		}
	}

	if (ferror(fp_in)) {
		perror(filename);
		status = 0;
	}

	free(threads);
	free(bwt_data);
	free(data);

	return status ? EXIT_SUCCESS : EXIT_FAILURE;
}

static int bwt_decompress(FILE *const restrict fp_in, FILE *const restrict fp_out, const unsigned short thread_count)
{
	bwt_size_t main_block_size;

	stats.curr_fs_in = fread(&main_block_size, 1, 1, fp_in);

	if (stats.curr_fs_in != 1) {
		return EXIT_SUCCESS;
	}

	main_block_size = 1U << main_block_size;

	unsigned char *const data = malloc(main_block_size * thread_count);

	if (!data) {
		fprintf(stderr, "%s: Not enough memory.\n", filename);

		return EXIT_FAILURE;
	}

	size_t n, status = 1;
	unsigned short i = 0;

	struct bwt_data_t *const restrict bwt_data = malloc(sizeof(struct bwt_data_t) * thread_count);
	thread_t *const restrict threads = malloc(sizeof(thread_t) * thread_count);

	while (fread(&bwt_data[i].header, sizeof(struct header_t), 1, fp_in) == 1) {
		bwt_data[i].data = data + main_block_size * i;

		if (bwt_data[i].header.block_size) {
			status = fread(bwt_data[i].data, 1, bwt_data[i].header.block_size, fp_in);

			if (status == bwt_data[i].header.block_size) {
				stats.curr_fs_in += status + sizeof(struct header_t);
				bwt_data[i].status = 1;
			} else {
				if (ferror(fp_in)) {
					perror(filename);
				} else {
					fprintf(stderr, "%s: Invalid input file format.\n", filename);
				}

				status = 0;

				break;
			}
		} else {
			bwt_data[i].header.block_size = fread(bwt_data[i].data, 1, main_block_size, fp_in);

			if (ferror(fp_in)) {
				perror(filename);
				status = 0;

				break;
			} else {
				stats.curr_fs_in += bwt_data[i].header.block_size + sizeof(struct header_t);
				bwt_data[i].status = 0;
			}
		}

#ifndef _WIN32
		pthread_create(&threads[i], NULL, threaded_decompress, &bwt_data[i]);
#else
		threads[i] = (thread_t) _beginthreadex(NULL, 0, threaded_decompress, &bwt_data[i], 0, NULL);
#endif
		i++;

		if (i == thread_count) {
			for (i = n = 0; i < thread_count; i++) {
#ifndef _WIN32
				pthread_join(threads[i], NULL);
#else
				WaitForSingleObject(threads[i], INFINITE);
				CloseHandle(threads[i]);
#endif
				n += bwt_data[i].header.block_size;
			}

			status = fwrite(data, 1, n, fp_out);

			if (status == n) {
				stats.curr_fs_out += n;
				i = 0;
			} else {
				perror(filename);
				status = 0;

				break;
			}
		}
	}

	if (ferror(fp_in)) {
		perror(filename);
		status = 0;
	} else if (status && i) {
		unsigned short j;

		for (j = n = 0; j < i; j++) {
#ifndef _WIN32
			pthread_join(threads[j], NULL);
#else
			WaitForSingleObject(threads[j], INFINITE);
			CloseHandle(threads[j]);
#endif
			n += bwt_data[j].header.block_size;
		}

		status = fwrite(data, 1, n, fp_out);

		if (status == n) {
			stats.curr_fs_out += n;
		} else {
			perror(filename);
			status = 0;
		}
	}

	free(threads);
	free(bwt_data);
	free(data);

	return status ? EXIT_SUCCESS : EXIT_FAILURE;
}

static void create_output_path(char *restrict input, char *const output, const unsigned char dec_flag)
{
	if (!input) {
		output[0] = 0;

		return;
	}

	const unsigned char ext_len = strlen(FILE_EXT);

#ifndef _WIN32
	strncat(output, basename(input), NAME_MAX - ext_len);
#else
	char input_name[_MAX_FNAME + 1], input_ext[_MAX_EXT + 1];

	_splitpath(input, NULL, NULL, input_name, input_ext);
	strcat(input_name, input_ext);
	strncat(output, input_name, _MAX_FNAME - ext_len);
#endif

	if (dec_flag) {
		char *const ext_pos = strstr(output + strlen(output) - ext_len, FILE_EXT);

		if (ext_pos) {
			*ext_pos = 0;
		}
	} else {
		strcat(output, FILE_EXT);
	}
}

static size_t get_filesize(FILE *const restrict fp)
{
	fseek(fp, 0, SEEK_END);
	const size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return size;
}

static unsigned short get_threadcount(void)
{
#ifndef _WIN32
	return sysconf(_SC_NPROCESSORS_ONLN);
#else
	SYSTEM_INFO sysinfo;

	GetSystemInfo(&sysinfo);

	return sysinfo.dwNumberOfProcessors;
#endif
}

static size_t get_memusage(void)
{
#ifndef _WIN32
	FILE *const restrict fp = fopen("/proc/self/statm", "rb");

	if (!fp) {
		return 0;
	}

	unsigned long vm_rss;
	const unsigned short page_size = sysconf(_SC_PAGESIZE);

	if (fscanf(fp, "%*u %lu", &vm_rss) != 1) {
		vm_rss = 0;
	}

	fclose(fp);

	return vm_rss * page_size;
#else
	PROCESS_MEMORY_COUNTERS pmc;

	if (!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
		return 0;
	}

	return pmc.WorkingSetSize;
#endif
}

#ifndef _WIN32
static void sighandler(const int signum)
{
	time(&stats.end_time);
	show_statistics(signum);
}
#else
static short getopt(const unsigned short argc, char **const restrict argv, const char *const restrict args)
{
	char curr_arg;
	const char *is_arg;
	static unsigned short i = 0;

	while (++i < argc) {
		optarg = NULL;

		if (argv[i][0] != '-') {
			if (!optind) {
				optind = i;
			}

			continue;
		}

		if (!(curr_arg = argv[i][1])) {
			fprintf(stderr, "%s: Missing argument.\n", filename);

			return '?';
		}

		if (!(is_arg = strchr(args, curr_arg))) {
			fprintf(stderr, "%s: Unknown argument -%c.\n", filename, curr_arg);

			return '?';
		}

		if (*(++is_arg) == ':') {
			if (argv[i + 1] && argv[i + 1][0] != '-') {
				optarg = argv[++i];
			} else if (*(++is_arg) != ':') {
				fprintf(stderr, "%s: Argument -%c requires a value.\n", filename, curr_arg);

				return '?';
			}
		}

		return curr_arg;
	}

	if (!optind) {
		optind = argc;
	}

	return -1;
}
#endif

static void show_statistics(const int signum)
{
	char time_buffer[] = "00m:00s";
	size_t diff_fs = 0;
	float diff_perc = 0, ratio = 0, speed = 0;

	const time_t diff_time = difftime(stats.end_time, stats.start_time);

	if (diff_time) {
		const struct tm *const restrict time_info = localtime(&diff_time);
		strftime(time_buffer, sizeof(time_buffer), "%Mm:%Ss", time_info);

		speed = stats.curr_fs_in / (1024.0 * 1024.0 * diff_time);
	}

	if (stats.curr_fs_in && stats.curr_fs_out) {
		if (stats.curr_fs_in > stats.curr_fs_out) {
			diff_fs = stats.curr_fs_in - stats.curr_fs_out;
			diff_perc = (100.0 * diff_fs) / stats.curr_fs_in;
		} else {
			diff_fs = stats.curr_fs_out - stats.curr_fs_in;
			diff_perc = (100.0 * diff_fs) / stats.curr_fs_out;
		}

		ratio = (double) stats.curr_fs_in / stats.curr_fs_out;
	}

	if (signum && stats.filesize_in) {
		const unsigned short progress = (100 * stats.curr_fs_in) / stats.filesize_in;
		fprintf(stderr, "Progress: %hu%%\n", progress);
	}

	fprintf(stderr, "Bytes read: %zu B\n"
					"Bytes written: %zu B\n"
					"Diff: %zu B (%.2f%%)\n"
					"Ratio: %.2f\n"
					"Time: %s\n"
					"Speed: %.2f MB/s\n",
			stats.curr_fs_in,
			stats.curr_fs_out,
			diff_fs,
			diff_perc,
			ratio,
			time_buffer,
			speed);

	if (signum) {
		const float memory = get_memusage() / (1024.0 * 1024.0);
		fprintf(stderr, "Memory (RAM): %.2f MB\n\n", memory);
	}
}

static void show_help(void)
{
	const unsigned short arch = sizeof(void *) * 8;
	const unsigned short thread_count = get_threadcount();

	fprintf(stderr, "%s - Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor.\n"
					"Build date: " __DATE__ " @ " __TIME__ "\n"
					"Architecture: %hu-bit\n\n"
					"Usage: %s [OPTIONS] [<input_file>]\n\n"
					"If <input_file> is omitted, input file is STDIN.\n"
					"If <input_file> is STDIN and output file is omitted (no valid -%c flag), output file is STDOUT (like -%c flag).\n"
#ifndef _WIN32
					"Other processes can send signal SIGUSR1 to get progress info.\n"
#endif
					"\nPossible options (can be combined together):\n"
					"\t-%c - Write output to STDOUT (ignored if valid -%c flag exists).\n"
					"\t-%c - Decompression mode.\n"
					"\t-%c - Show help and exit.\n"
					"\t-%c <1-%hu> - Number of parallel jobs (threads). If omitted or wrong value, fallback to available threads (%hu).\n"
					"\t-%c <output_name> - Custom output filename (if omitted, output filename is input_file" FILE_EXT " in compression and without " FILE_EXT " in decompression mode).\n"
					"\t\t- If given string ends with '/' (path delimiter), output is then given path + above rule.\n"
					"\t-%c <%u-%u> - Compression preset level (ignored in decompression mode).\n"
					"\t\t- Higher presets may give better compression ratio (if omitted or wrong value, preset is %u).\n"
					"\t-%c - Remove input file after successful operation (ignored when input file is STDIN).\n"
					"\t-%c - Verbose mode - show statistics after successful operation.\n",
			filename,
			arch,
			filename,
			OUTPUT_FLAG,
			STDOUT_FLAG,
			STDOUT_FLAG,
			OUTPUT_FLAG,
			DEC_FLAG,
			HELP_FLAG,
			JOBS_FLAG,
			thread_count,
			thread_count,
			OUTPUT_FLAG,
			PRESET_FLAG,
			PRESET_MIN,
			PRESET_MAX,
			PRESET_DEF,
			REMOVE_FLAG,
			VERBOSE_FLAG);
}

int main(const int argc, char **argv)
{
	time(&stats.start_time);

	int c;
	unsigned long jobs = 0, block_size = PRESET_DEF;
	char *input, output[MAX_PATH + 1] = {0};
	struct flags_t flags = {0};
	FILE *fp_in, *fp_out = NULL;

#ifndef _WIN32
	freopen(NULL, FOPEN_INPUT_MODE, stdin);
	freopen(NULL, FOPEN_OUTPUT_MODE, stdout);
#else
	_setmode(_fileno(stdin), _O_BINARY);
	_setmode(_fileno(stdout), _O_BINARY);
#endif

	fp_in = stdin;

#ifndef _WIN32
	filename = basename(argv[0]);
#else
	_splitpath(argv[0], NULL, NULL, filename, NULL);
#endif

	while ((c = getopt(argc, argv, ARGS)) != -1) {
		switch (c) {
			case HELP_FLAG: {
				show_help();

				return EXIT_SUCCESS;
			}
			case DEC_FLAG: {
				flags.dec = 1;

				break;
			}
			case REMOVE_FLAG: {
				flags.remove = 1;

				break;
			}
			case VERBOSE_FLAG: {
				flags.verbose = 1;

				break;
			}
			case PRESET_FLAG: {
				block_size = strtoul(optarg, NULL, 10);

				break;
			}
			case JOBS_FLAG: {
				jobs = strtoul(optarg, NULL, 10);

				break;
			}
			case STDOUT_FLAG: {
				fp_out = stdout;

				break;
			}
			case OUTPUT_FLAG: {
				strcpy(output, optarg);

				break;
			}
			default: {
				fprintf(stderr, "Run %s -%c for help.\n", filename, HELP_FLAG);

				return EXIT_FAILURE;
			}
		}
	}

	input = argv[optind];

	if (!fp_out && !output[0]) {
		if (input) {
			create_output_path(input, output, flags.dec);
		} else {
			fp_out = stdout;
		}
	}

	if (input) {
		fp_in = fopen(input, FOPEN_INPUT_MODE);

		if (!fp_in) {
			perror(filename);

			return EXIT_FAILURE;
		}

		stats.filesize_in = get_filesize(fp_in);

		if (!stats.filesize_in) {
			fclose(fp_in);

			return EXIT_SUCCESS;
		}
	} else {
		flags.remove = 0;
	}

	if (output[0]) {
		const unsigned short output_len = strlen(output);

#ifndef _WIN32
		if (output[output_len - 1] == '/') {
			create_output_path(input, output, flags.dec);
		}
#else
		if (output[output_len - 1] == '/' || output[output_len - 1] == '\\') {
			create_output_path(input, output, flags.dec);
		}
#endif

		if (!output[0]) {
			fp_out = stdout;
		} else {
			if (input && !strcmp(input, output)) {
				fprintf(stderr, "%s: Input and output file must be different.\n", filename);
				fclose(fp_in);

				return EXIT_FAILURE;
			}

			fp_out = fopen(output, FOPEN_OUTPUT_MODE);

			if (!fp_out) {
				perror(filename);
				fclose(fp_in);

				return EXIT_FAILURE;
			}
		}
	}

	int status;
	unsigned short thread_count = get_threadcount();

	if (jobs && jobs < thread_count) {
		thread_count = jobs;
	}

#ifndef _WIN32
	signal(SIGTYPE, sighandler);
#endif

	if (flags.dec) {
		status = bwt_decompress(fp_in, fp_out, thread_count);
	} else {
		if (block_size < PRESET_MIN || block_size > PRESET_MAX) {
			block_size = PRESET_DEF;
		}

		status = bwt_compress(fp_in, fp_out, thread_count, block_size + SIZE_THRESH);
	}

	fclose(fp_in);
	fclose(fp_out);

	if (status == EXIT_SUCCESS) {
		if (flags.remove) {
			remove(input);
		}

		if (flags.verbose) {
			time(&stats.end_time);
			show_statistics(0);
		}
	} else if (output[0]) {
		remove(output);
	}

	return status;
}
