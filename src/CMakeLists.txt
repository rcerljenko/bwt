cmake_minimum_required(VERSION 3.1)

project(bwt)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast -Wall -Wextra -s -m64 -march=native -Wl,-O1,-s -mfpmath=sse")

link_directories(libbwt/build)
add_executable(bwt bwt.c)
set_property(TARGET bwt PROPERTY C_STANDARD 11)
target_link_libraries(bwt -lbwt pthread)

install(TARGETS bwt RUNTIME DESTINATION bin)