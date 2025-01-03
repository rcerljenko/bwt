cmake_minimum_required(VERSION 3.21)

add_subdirectory(libbwt libbwt)

project(pbwt C)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 23)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	add_compile_options(-Wall -Wextra -march=native -Ofast -flto)
	add_link_options(-s)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Weverything -Ofast -flto)
	add_link_options(-s)
elseif(CMAKE_C_COMPILER_ID MATCHES "Intel")
	if(WIN32)
		add_compile_options(/Wall /O2 /Ob3 /Qipo /Qrestrict /Qpar /Tc)
	else()
		add_compile_options(-Wall -Ofast -ipo -restrict -parallel)
		add_link_options(-s)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /O2 /Ob3 /Gd /GL /Gw /Qpar /Tc)
endif()

if(APPLE)
	set(CMAKE_MACOSX_RPATH OFF)
endif()

add_executable(pbwt pbwt.c)

if(UNIX)
	target_link_libraries(pbwt bwt pthread)
else()
	target_link_libraries(pbwt bwt)
endif()

install(TARGETS pbwt RUNTIME)
