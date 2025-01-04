cmake_minimum_required(VERSION 3.21)

add_subdirectory(libbwt libbwt)

project(pbwt C)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_STANDARD 23)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
	add_compile_options(-Wall -Wextra -march=native -Ofast -flto)

	if(NOT APPLE)
		add_link_options(-s -flto)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Weverything -O3 -flto)

	if(NOT APPLE)
		add_link_options(-fuse-ld=lld)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "Intel")
	if(WIN32)
		add_compile_options(/Wall /O2 /Ob3 /Qipo /Qpar /Tc)
		add_link_options(/LTCG)
	else()
		add_compile_options(-Wall -Ofast -ipo -parallel)
	endif()
elseif(CMAKE_C_COMPILER_ID MATCHES "MSVC")
	add_compile_options(/Wall /O2 /Ob3 /Gd /GL /Gw /Qpar /Tc)
	add_link_options(/LTCG)
endif()

add_executable(pbwt pbwt.c)

if(UNIX)
	if(APPLE)
		set_target_properties(pbwt PROPERTIES INSTALL_RPATH "${CMAKE_CURRENT_BINARY_DIR}/libbwt")
	endif()

	target_link_libraries(pbwt bwt pthread)
else()
	target_link_libraries(pbwt bwt)
endif()

install(TARGETS pbwt RUNTIME)
