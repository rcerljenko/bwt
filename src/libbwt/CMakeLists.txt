cmake_minimum_required(VERSION 3.30)

project(libbwt C)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(CMAKE_BUILD_TYPE "Build type" Release)

set(CMAKE_BUILD_PARALLEL_LEVEL "")
set(INSTALL_PARALLEL ON)
set(CMAKE_C_STANDARD 23)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	add_compile_options(-Wall -Wextra -Wpedantic -march=native)

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(-Ofast)

		if(NOT APPLE)
			add_link_options(-s)
		endif()
	else()
		add_compile_options(-g3)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	if(MSVC)
		add_compile_options(/Wall /Gd /Qvec)

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Z7")
	else()
		add_compile_options(-Weverything)

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3")
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "IntelLLVM")
	if(MSVC)
		add_compile_options(/Wall /Gd /QxHost)

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O3 /Ob3 /Ox /Qvec")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Z7")
	else()
		add_compile_options(-Weverything -xHost)

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -vec")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3")
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /sdl /permissive- /Gd)

	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Ob3 /Qpar")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Z7")
endif()

add_library(bwt bwt.c)

if(WIN32 AND NOT BUILD_SHARED_LIBS)
	target_compile_definitions(bwt PUBLIC BWT_STATIC)
endif()

set_target_properties(bwt PROPERTIES PUBLIC_HEADER bwt.h)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	include(CheckIPOSupported)

	check_ipo_supported(RESULT IPO_SUPPORTED)

	if(IPO_SUPPORTED)
		set_target_properties(bwt PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	else()
		message(WARNING "IPO is not supported.")
	endif()
endif()

install(TARGETS bwt LIBRARY)
install(TARGETS bwt PUBLIC_HEADER)

if(LINUX)
	install(CODE "execute_process(COMMAND ldconfig)")
endif()
