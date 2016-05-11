
Build: gcc -std=gnu11 -Wall -Wextra -Ofast -Wl,-O1 -s -m64 -march=native -mfpmath=sse -pthread bwt.c -o bwt


Header format:


At the begining of file:

1 B - block size used for compression, power using base 2 (i.e. 12 = 2^12 = 4096 B).

Before every block:

4 B - block size of current block (set to zero if rle() failed, then in decompression main_block_size is used for ibwt()).
4 B - BWT index position of original rotation.