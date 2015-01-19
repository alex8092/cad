cmake_minimum_required(VERSION 2.8)

project(cad)

SET(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Werror")
SET(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
SET(CMAKE_C_COMPILER "g++")
include_directories(
	include/
)

file(
	GLOB_RECURSE
	header_files
	include/*
)

file(
	GLOB_RECURSE
	source_files
	src/*
)

add_executable(
	cad
	${header_files}
	${source_files}
)
