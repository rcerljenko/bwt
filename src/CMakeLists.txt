cmake_minimum_required(VERSION 3.30)

option(BUILD_BWT_LIBRARY "Build BWT library" ON)

if(BUILD_BWT_LIBRARY)
	add_subdirectory(libbwt)
endif()

project(pbwt
	VERSION 1.0.0
	DESCRIPTION "Portable Multithreaded Burrows-Wheeler Transform + Run Length Encoding Compressor/Decompressor"
	HOMEPAGE_URL "https://github.com/rcerljenko/bwt"
	LANGUAGES C
)

option(BWT_NO_MTF "Build without MTF pass")
option(USE_ASAN "Use ASAN checks")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_BUILD_PARALLEL_LEVEL "")
set(INSTALL_PARALLEL ON)
set(CMAKE_LINK_WARNING_AS_ERROR TRUE)
set(CMAKE_C_STANDARD 23)

set(CPACK_THREADS 0)
set(CPACK_STRIP_FILES TRUE)
set(CPACK_PACKAGE_CHECKSUM SHA3_512)
set(CPACK_PACKAGE_VENDOR "Ricardo Cerljenko")
set(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})

if(LINUX)
	set(CPACK_DEBIAN_COMPRESSION_TYPE zstd)
	set(CPACK_COMPRESSION_LEVEL 19)
	set(CPACK_GENERATOR DEB)
else()
	set(CPACK_COMPRESSION_LEVEL 9)
	set(CPACK_GENERATOR ZIP)
endif()

include(CPack)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	add_compile_options(-Wall -Wextra -Wpedantic -march=native)

	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3")

	if(USE_ASAN)
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address,pointer-compare,pointer-subtract,leak,undefined")
	endif()

	if(NOT APPLE AND CMAKE_BUILD_TYPE STREQUAL "Release")
		add_link_options(-s)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	if(MSVC)
		add_compile_options(/Wall /Gd)

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Qvec")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Z7")

		if(USE_ASAN)
			set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /fsanitize=address")
		endif()
	else()
		add_compile_options(-Weverything)

		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3")

		if(USE_ASAN)
			set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address,leak,undefined")
		endif()
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
	add_compile_options(/Wall /permissive- /Gd)

	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /Ob3 /Qpar")
	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Z7")
endif()

add_executable(pbwt pbwt.c utils/utils.c)

if(BWT_NO_MTF)
	target_compile_definitions(pbwt PUBLIC BWT_NO_MTF)

	message(WARNING "Building without MTF pass.")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	include(CheckIPOSupported)

	check_ipo_supported(RESULT IPO_SUPPORTED)

	if(IPO_SUPPORTED)
		set_target_properties(pbwt PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	else()
		message(WARNING "IPO is not supported.")
	endif()
elseif(MSVC)
	set_target_properties(bwt PROPERTIES MSVC_RUNTIME_CHECKS "PossibleDataLoss,StackFrameErrorCheck,UninitializedVariable")
endif()

if(UNIX)
	if(APPLE)
		set_target_properties(pbwt PROPERTIES INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/libbwt;${CMAKE_CURRENT_BINARY_DIR}/libbwt/${CMAKE_BUILD_TYPE}")
	endif()

	target_link_libraries(pbwt bwt pthread)

	include(GNUInstallDirs)
	install(FILES ../man/pbwt.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
else()
	target_link_libraries(pbwt bwt)
endif()

install(TARGETS pbwt RUNTIME)
