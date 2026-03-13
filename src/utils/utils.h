#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define FOPEN_INPUT_MODE "rb"
#define FOPEN_OUTPUT_MODE "wb"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
extern const char *optarg;
extern unsigned short optind;
#endif

size_t get_filesize(FILE *const restrict fp);
unsigned short get_threadcount(void);
size_t get_memusage(void);
int set_streams_to_binary_mode(void);
#ifdef _WIN32
short getopt(const unsigned short argc, char **const restrict argv, const char *const restrict args);
#endif

#ifdef __cplusplus
}
#endif
#endif
