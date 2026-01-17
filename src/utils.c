#ifndef _WIN32
	#include <unistd.h>
#else
	#include <windows.h>
	#include <psapi.h>
	#include <fcntl.h>
	#include <io.h>
#endif

#include <stdlib.h>
#include <string.h>
#include "utils.h"

#ifndef _WIN32
extern char *filename;
#else
extern char filename[_MAX_FNAME + 1];
const char *optarg;
unsigned short optind;
#endif

size_t get_filesize(FILE *const restrict fp)
{
	const size_t offset = ftell(fp);

	fseek(fp, 0, SEEK_END);

	const size_t size = ftell(fp);

	fseek(fp, offset, SEEK_SET);

	return size;
}

unsigned short get_threadcount(void)
{
#ifndef _WIN32
	return sysconf(_SC_NPROCESSORS_ONLN);
#else
	SYSTEM_INFO sysinfo;

	GetSystemInfo(&sysinfo);

	return sysinfo.dwNumberOfProcessors;
#endif
}

size_t get_memusage(void)
{
#ifndef _WIN32
	FILE *const restrict fp = fopen("/proc/self/statm", FOPEN_INPUT_MODE);

	if (!fp) {
		perror(filename);

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

int set_streams_to_binary_mode(void)
{
#ifndef _WIN32
	FILE *fp;

	fp = freopen(NULL, FOPEN_INPUT_MODE, stdin);

	if (!fp) {
		perror(filename);

		return EXIT_FAILURE;
	}

	fp = freopen(NULL, FOPEN_OUTPUT_MODE, stdout);

	if (!fp) {
		perror(filename);

		return EXIT_FAILURE;
	}
#else
	int mode;

	mode = _setmode(_fileno(stdin), _O_BINARY);

	if (mode == -1) {
		perror(filename);

		return EXIT_FAILURE;
	}

	mode = _setmode(_fileno(stdout), _O_BINARY);

	if (mode == -1) {
		perror(filename);

		return EXIT_FAILURE;
	}
#endif

	return EXIT_SUCCESS;
}

#ifdef _WIN32
short getopt(const unsigned short argc, char **const restrict argv, const char *const restrict args)
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
