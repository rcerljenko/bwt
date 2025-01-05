cmake_minimum_required(VERSION 3.21)

add_subdirectory(libbwt libbwt)

project(pbwt C)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 23)

include(CheckIPOSupported)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	add_compile_options(-Wall -Wextra -march=native -Ofast)

	if(NOT APPLE)
		add_link_options(-s)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Weverything -O3)
elseif(CMAKE_C_COMPILER_ID MATCHES "IntelLLVM")
	if(WIN32)
		add_compile_options(/Wall /O3 /Ox /Ob3 /Gd /Qpar /Tc /QxHost)
	else()
		add_compile_options(-Weverything -O3 -xHost)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Intel")
	if(WIN32)
		add_compile_options(/Wall /O3 /Ox /Ob3 /Gd /Qpar /Tc /Qrestrict /QxHost)
	else()
		add_compile_options(-Wall -Ofast -parallel -restrict -xHost)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /O2 /Ob3 /Gd /Qpar /Tc)
endif()

add_executable(pbwt pbwt.c)

check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT IPO_OUTPUT)
if(IPO_SUPPORTED)
	set_target_properties(pbwt PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
	message(WARNING "IPO is not supported: ${IPO_OUTPUT}")
endif()

if(UNIX)
	if(APPLE)
		set_target_properties(pbwt PROPERTIES INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/libbwt")
	endif()

	target_link_libraries(pbwt bwt pthread)
else()
	target_link_libraries(pbwt bwt)
endif()

install(TARGETS pbwt RUNTIME)
