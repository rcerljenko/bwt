CC=gcc
CFLAGS="-std=gnu11 -Wall -Wextra -s -march=native -Ofast -flto"
LIB_PATH=`pwd`/libbwt

$CC $CFLAGS -fPIC -Wl,-soname,libbwt.so -shared -o $LIB_PATH/libbwt.so $LIB_PATH/libbwt.c
$CC $CFLAGS -Wl,-rpath=$LIB_PATH -L $LIB_PATH -o pbwt pbwt.c -pthread -lbwt