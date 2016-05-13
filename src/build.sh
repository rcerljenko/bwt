CC=gcc
CFLAGS="-Ofast -std=gnu11 -Wall -Wextra -s -m64 -march=native -Wl,-O1,-s"
LIB_PATH=`pwd`

$CC $CFLAGS -fPIC -shared -o libbwt.so libbwt.c
$CC $CFLAGS -mfpmath=sse -pthread -Wl,-rpath=$LIB_PATH -L $LIB_PATH -o bwt bwt.c -lbwt