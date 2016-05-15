cmake_minimum_required(VERSION 3.1)

project(pbwt)
set(CMAKE_BUILD_TYPE Release)

if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra -s -m64 -march=native -mfpmath=sse -Ofast -flto")
endif(CMAKE_COMPILER_IS_GNUCC)

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