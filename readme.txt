
Build: gcc -Wall -Wextra -s -m64 -march=native -msse4.2 -mfpmath=sse -lm -pthread bwt.c -o bwt


Header format:


At the begining of file:

1 B - block size used for compression, power using base 2 (i.e. 12 = 2^12 = 4096 B).

Before every block:

2 B - block size of current block (set to zero if rle() failed, then in decompression main_block_size is used for ibwt()).
2 B - BWT index position of original rotation.
