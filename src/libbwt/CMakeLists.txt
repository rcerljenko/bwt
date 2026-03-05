cmake_minimum_required(VERSION 3.30)

project(libbwt C)

option(BUILD_SHARED_LIBS "Build shared library" ON)
option(CMAKE_BUILD_TYPE "Build type" Release)

set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
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

		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			add_compile_options(/O2)
		else()
			add_compile_options(/Z7)
		endif()
	else()
		add_compile_options(-Weverything)

		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			add_compile_options(-O3)
		else()
			add_compile_options(-g3)
		endif()
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "IntelLLVM")
	if(MSVC)
		add_compile_options(/Wall /Gd /QxHost)

		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			add_compile_options(/O3 /Ob3 /Ox /Qvec)
		else()
			add_compile_options(/Z7)
		endif()
	else()
		add_compile_options(-Weverything -xHost)

		if(CMAKE_BUILD_TYPE STREQUAL "Release")
			add_compile_options(-O3 -vec)
		else()
			add_compile_options(-g3)
		endif()
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /sdl /permissive- /Gd)

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		add_compile_options(/O2 /Ob3 /Qpar)
	else()
		add_compile_options(/Z7)
	endif()
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
