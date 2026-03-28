cmake_minimum_required(VERSION 3.16)

project(TicketSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(code
	src/main.cpp
)

target_include_directories(code
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(code PRIVATE -Wall -Wextra -Wpedantic)
endif()
