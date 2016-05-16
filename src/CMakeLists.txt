cmake_minimum_required(VERSION 3.1)

project(pbwt)
set(CMAKE_BUILD_TYPE Release)

if("${CMAKE_C_COMPILER_ID}" MATCHES "GNU")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -s -m64 -march=native -Ofast -flto")
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Weverything -O4 -Wl,-s")
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "Intel")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -W5 -m64 -O3 -ipo")
elseif("${CMAKE_C_COMPILER_ID}" MATCHES "MSVC")
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Wall /O2 /Ot /Gd /GL /Gw /Qpar /Tc")
endif()

set(REQUIRED_LIBS bwt)
if(UNIX)
	set(REQUIRED_LIBS "${REQUIRED_LIBS} pthread")
endif(UNIX)

link_directories(libbwt/build)
add_executable(pbwt pbwt.c)
set_property(TARGET pbwt PROPERTY C_STANDARD 11)
target_link_libraries(pbwt ${REQUIRED_LIBS})

if(UNIX)
	install(TARGETS pbwt RUNTIME DESTINATION bin)
endif(UNIX)