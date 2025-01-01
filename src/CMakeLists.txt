cmake_minimum_required(VERSION 3.21)

add_subdirectory(libbwt libbwt)

project(pbwt C)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 23)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -march=native -Ofast -flto")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Weverything -Ofast -flto")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
elseif(CMAKE_C_COMPILER_ID MATCHES "Intel")
	if(WIN32)
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Wall /Ox /GF /Qipo /Qrestrict /Qpar /Tc")
	else()
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Ofast -ipo -restrict -parallel")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Wall /Ox /GF /Gd /GL /Gw /Qpar /Tc")
endif()

find_library(SEARCH bwt)
if(SEARCH MATCHES "-NOTFOUND")
	link_directories(libbwt/build)
endif()

add_executable(pbwt pbwt.c)

if(UNIX)
	target_link_libraries(pbwt bwt pthread)
else()
	target_link_libraries(pbwt bwt)
endif()

install(TARGETS pbwt RUNTIME)
