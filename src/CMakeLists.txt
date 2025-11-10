cmake_minimum_required(VERSION 3.25)

add_subdirectory(libbwt)

project(pbwt
	VERSION 1.0.0
	DESCRIPTION "Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor"
	HOMEPAGE_URL "https://github.com/rcerljenko/bwt"
	LANGUAGES C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 23)
set(CMAKE_BUILD_PARALLEL_LEVEL "")
set(INSTALL_PARALLEL ON)

set(CPACK_THREADS 0)
set(CPACK_STRIP_FILES TRUE)
set(CPACK_PACKAGE_CHECKSUM SHA3_512)
set(CPACK_PACKAGE_VENDOR "Ricardo Cerljenko")
set(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})

if(LINUX)
	set(CPACK_GENERATOR DEB)
else()
	set(CPACK_GENERATOR ZIP)
endif()

include(CPack)
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
		add_compile_options(/Wall /O3 /Ox /Ob3 /Gd /Qpar /Qvec /QxHost)
	else()
		add_compile_options(-Weverything -O3 -vec -xHost)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /sdl /permissive- /O2 /Ob3 /Gd /Qpar)
endif()

add_executable(pbwt pbwt.c)

check_ipo_supported(RESULT IPO_SUPPORTED)

if(IPO_SUPPORTED)
	set_target_properties(pbwt PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
	message(WARNING "IPO is not supported.")
endif()

if(UNIX)
	if(APPLE)
		set_target_properties(pbwt PROPERTIES INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/libbwt;${CMAKE_CURRENT_BINARY_DIR}/libbwt/${CMAKE_BUILD_TYPE}")
	endif()

	target_link_libraries(pbwt bwt pthread)
else()
	target_link_libraries(pbwt bwt)
endif()

install(TARGETS pbwt RUNTIME)
