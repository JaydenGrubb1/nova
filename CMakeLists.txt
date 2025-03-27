# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project(nova)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration Options
set(NOVA_BUILD_ENGINE ON CACHE BOOL "Build the engine")
set(NOVA_BUILD_EDITOR ON CACHE BOOL "Build the editor")
set(NOVA_ENGINE_SHARED ON CACHE BOOL "Build the engine as a shared library")
set(NOVA_ENGINE_STATIC OFF CACHE BOOL "Build the engine as a static library")
set(NOVA_EDITOR_STATIC OFF CACHE BOOL "Link the editor against the engine statically")

if (NOVA_BUILD_EDITOR)
	if (NOVA_EDITOR_STATIC)
		set(NOVA_BUILD_ENGINE ON)
		set(NOVA_ENGINE_STATIC ON)
	else ()
		set(NOVA_ENGINE_SHARED ON)
	endif ()
endif ()

if (MSVC)
	add_compile_options(
		/W3
		/permissive-
		/EHsc
	)
else ()
	add_compile_options(
		-Wall
		-Wextra
		-Wpedantic
	)
endif ()

if (WIN32)
	add_compile_definitions(
		_CRT_SECURE_NO_WARNINGS
		NOMINMAX
		WIN32_LEAN_AND_MEAN
	)
endif ()

if (NOVA_BUILD_ENGINE)
	add_subdirectory(engine)
endif ()
if (NOVA_BUILD_EDITOR)
	add_subdirectory(editor)
endif ()
