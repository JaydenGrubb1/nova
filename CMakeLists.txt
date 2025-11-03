# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project(nova VERSION 0.0.1)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_compile_definitions(
	NOVA_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
	NOVA_VERSION_MINOR=${PROJECT_VERSION_MINOR}
	NOVA_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options(
		/W4
		/permissive-
		/EHsc
	)
	add_compile_definitions(
		NOVA_COMPILER_MSVC
	)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_compile_options(
		-Wall
		-Wextra
		-Wpedantic
		-Wshadow
		-Wold-style-cast
	)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		add_compile_definitions(
			NOVA_COMPILER_GCC
		)
	else()
		add_compile_definitions(
			NOVA_COMPILER_CLANG
		)
	endif()
else()
	error("Unsupported compiler")
endif()

if (WIN32)
	add_compile_definitions(
		NOVA_PLATFORM_WINDOWS
		NOMINMAX
		WIN32_LEAN_AND_MEAN
		_CRT_SECURE_NO_WARNINGS
		_CRT_NONSTDC_NO_WARNINGS
	)
elseif (APPLE)
	add_compile_definitions(
		NOVA_PLATFORM_MACOS
	)
elseif (UNIX)
	add_compile_definitions(
		NOVA_PLATFORM_LINUX
	)
else()
	error("Unsupported platform")
endif()

add_subdirectory(engine)
add_subdirectory(editor)
