cmake_minimum_required(VERSION 3.1)

project(libbwt C)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 11)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -march=native -Ofast -flto")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Weverything -Ofast -flto")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
elseif(CMAKE_C_COMPILER_ID MATCHES "Intel")
	if(WIN32)
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Wall /Ox /GF /Qipo /Qrestrict /Qpar /Tc")
	else()
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Ofast -ipo -restrict -parallel")
		set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Wall /Ox /GF /Gd /GL /Gw /Qpar /Tc")
endif()

add_library(bwt SHARED bwt.c)

if(UNIX OR APPLE)
	include(GNUInstallDirs)
	install(TARGETS bwt LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
	install(FILES bwt.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()