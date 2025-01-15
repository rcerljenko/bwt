cmake_minimum_required(VERSION 3.25)

project(libbwt C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 23)
set(CMAKE_BUILD_PARALLEL_LEVEL "")
set(INSTALL_PARALLEL ON)

option(BUILD_SHARED_LIBS "Build shared library" ON)

include(CheckIPOSupported)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	add_compile_options(-Wall -Wextra -march=native -Ofast)

	if(NOT APPLE)
		add_link_options(-s)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	if(MSVC)
		add_compile_options(/Wall /O2 /Gd /Qvec)
	else()
		add_compile_options(-Weverything -O3)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "IntelLLVM")
	if(WIN32)
		add_compile_options(/Wall /O3 /Ox /Ob3 /Gd /Qpar /QxHost)
	else()
		add_compile_options(-Weverything -O3 -xHost)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /O2 /Ob3 /Gd /Qpar)
endif()

add_library(bwt bwt.c)

if(WIN32 AND NOT BUILD_SHARED_LIBS)
	target_compile_definitions(bwt PUBLIC BWT_STATIC)
endif()

set_target_properties(bwt PROPERTIES PUBLIC_HEADER bwt.h)

check_ipo_supported(RESULT IPO_SUPPORTED)

if(IPO_SUPPORTED)
	set_target_properties(bwt PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
	message(WARNING "IPO is not supported.")
endif()

install(TARGETS bwt LIBRARY)
install(TARGETS bwt PUBLIC_HEADER)

if(APPLE)
	install(CODE "execute_process(COMMAND update_dyld_shared_cache)")
elseif(UNIX)
	install(CODE "execute_process(COMMAND ldconfig)")
endif()
