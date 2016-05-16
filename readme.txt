
Build:
- On Linux use build.sh script.
- On Windows set compiler flags to match commands below.
- Or use CMake on any platform.

Shared lib:

gcc -std=gnu11 -Wall -Wextra -s -m64 -march=native -Ofast -flto -shared -fPIC -o libbwt.so libbwt.c

App:

gcc -std=gnu11 -Wall -Wextra -s -m64 -march=native -Ofast -flto -Wl,-rpath=. -L. -o bwt bwt.c -pthread -lbwt


Header format:

At the begining of file:

1 B - block size used for compression, power using base 2 (i.e. 12 = 2^12 = 4096 B).

Before every block:

4 B - block size of current block (set to zero if rle() failed, then in decompression main_block_size is used for ibwt()).
4 B - BWT index position of original rotation.
