CC=gcc
CFLAGS="-Ofast -std=gnu11 -Wall -Wextra -s -m64 -march=native -Wl,-O1"
LIB_PATH=`pwd`

$CC $CFLAGS -shared -fPIC -o libbwt.so libbwt.c
$CC $CFLAGS -mfpmath=sse -pthread -Wl,-rpath=$LIB_PATH -L $LIB_PATH -o bwt bwt.c -lbwt