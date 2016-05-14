CC=gcc
CFLAGS="-Ofast -std=gnu11 -Wall -Wextra -s -m64 -march=native -Wl,-O1,-s"
LIB_PATH=`pwd`/libbwt

$CC $CFLAGS -fPIC -Wl,-soname=libbwt.so -shared -o $LIB_PATH/libbwt.so $LIB_PATH/libbwt.c
$CC $CFLAGS -mfpmath=sse -pthread -Wl,-rpath=$LIB_PATH -L $LIB_PATH -o bwt bwt.c -lbwt