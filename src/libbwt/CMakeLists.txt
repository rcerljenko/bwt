cmake_minimum_required(VERSION 3.21)

project(libbwt C)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 23)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	add_compile_options(-Wall -Wextra -march=native -Ofast)

	if(NOT APPLE)
		add_link_options(-s)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Weverything -O3)

	if(NOT APPLE)
		add_link_options(-fuse-ld=lld)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Intel")
	if(WIN32)
		add_compile_options(/Wall /O2 /Ob3 /Qpar /Tc)
	else()
		add_compile_options(-Wall -Ofast -parallel)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /O2 /Ob3 /Gd /Qpar /Tc)
endif()

add_library(bwt SHARED bwt.c)
set_target_properties(bwt PROPERTIES PUBLIC_HEADER bwt.h)

check_ipo_supported(RESULT IPO_SUPPORTED)
if(IPO_SUPPORTED)
	set_target_properties(bwt PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

install(TARGETS bwt LIBRARY)
install(TARGETS bwt PUBLIC_HEADER)

if(APPLE)
	install(CODE "execute_process(COMMAND update_dyld_shared_cache)")
elseif(UNIX)
	install(CODE "execute_process(COMMAND ldconfig)")
endif()
