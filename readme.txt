
Build:
- On Linux use build.sh script.
- On Windows set compiler flags to match commands below.

Shared lib:

gcc -Ofast -std=gnu11 -Wall -Wextra -s -m64 -march=native -Wl,-O1,-s -shared -fPIC -o libbwt.so libbwt.c

App:

gcc -Ofast -std=gnu11 -Wall -Wextra -s -m64 -march=native -Wl,-O1,-s -mfpmath=sse -pthread -Wl,-rpath=. -L. -o bwt bwt.c -lbwt


Header format:

At the begining of file:

1 B - block size used for compression, power using base 2 (i.e. 12 = 2^12 = 4096 B).

Before every block:

4 B - block size of current block (set to zero if rle() failed, then in decompression main_block_size is used for ibwt()).
4 B - BWT index position of original rotation.
