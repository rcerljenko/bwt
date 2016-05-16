cmake_minimum_required(VERSION 3.1)

project(libbwt C)
set(CMAKE_BUILD_TYPE Release)

if("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -s -m64 -march=native -Ofast -flto")
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Weverything -O4 -Wl,-s")
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "Intel")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -W5 -m64 -O3 -ipo")
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "MSVC")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Wall /O2 /Ot /Gd /GL /Gw /Qpar /Tc")
endif()

add_library(bwt SHARED libbwt.c)
set_property(TARGET bwt PROPERTY C_STANDARD 11)

if(UNIX OR APPLE)
	install(TARGETS bwt LIBRARY DESTINATION lib)
	install(FILES bwt.h DESTINATION include)
endif()