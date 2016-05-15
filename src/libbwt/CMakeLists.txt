cmake_minimum_required(VERSION 3.1)

project(libbwt)
set(CMAKE_BUILD_TYPE Release)

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -s -m64 -march=native -Ofast -flto")
endif(CMAKE_COMPILER_IS_GNUCC)

add_library(bwt SHARED libbwt.c)
set_property(TARGET bwt PROPERTY C_STANDARD 11)

if(UNIX)
	install(TARGETS bwt LIBRARY DESTINATION lib)
	install(FILES bwt.h DESTINATION include)
endif(UNIX)