# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project(nova VERSION 0.0.1)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(NOVA_SANITIZERS "" CACHE STRING "Sanitizers to enable (address, undefined, leak)")
set(NOVA_WERROR OFF CACHE BOOL "Treat compiler warnings as errors")
set(NOVA_LIBRARY_INSTALL OFF CACHE BOOL "Enable library installation targets")

# set(NOVA_BACKEND_DX12 ON CACHE BOOL "Enable DX12 backend")
# set(NOVA_BACKEND_METAL ON CACHE BOOL "Enable Metal backend")
set(NOVA_BACKEND_VULKAN ON CACHE BOOL "Enable Vulkan backend")
# set(NOVA_BACKEND_WEBGPU ON CACHE BOOL "Enable WebGPU backend")

set(NOVA_USE_VOLK ON CACHE BOOL "Use volk for Vulkan function loading")

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
		$<$<BOOL:${NOVA_WERROR}>:/WX>
	)

	if (NOVA_SANITIZERS)
		string(REPLACE "," ";" SANITIZERS ${NOVA_SANITIZERS})
		foreach(sanitizer IN LISTS SANITIZERS)
			add_compile_options(/fsanitize=${sanitizer})
		endforeach()
	endif()

	add_compile_definitions(
		NOVA_COMPILER_MSVC
		_CRT_SECURE_NO_WARNINGS
		_CRT_NONSTDC_NO_WARNINGS
	)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(
		-Wall
		-Wextra
		-Wpedantic
		-Wshadow
		-Wold-style-cast
		$<$<BOOL:${NOVA_WERROR}>:-Werror>
	)

	if (NOVA_SANITIZERS)
		add_compile_options(-fsanitize=${NOVA_SANITIZERS})
		add_link_options(-fsanitize=${NOVA_SANITIZERS})
	endif()

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		add_compile_definitions(NOVA_COMPILER_GCC)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		add_compile_definitions(NOVA_COMPILER_CLANG)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		add_compile_definitions(
			NOVA_COMPILER_CLANG
			NOVA_COMPILER_APPLE_CLANG
		)
	endif()
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_compile_definitions(
		NOVA_PLATFORM_WINDOWS
		NOMINMAX
		WIN32_LEAN_AND_MEAN
	)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_compile_definitions(NOVA_PLATFORM_MACOS)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	add_compile_definitions(NOVA_PLATFORM_LINUX)
else()
	message(FATAL_ERROR "Unsupported platform")
endif()

if (NOVA_LIBRARY_INSTALL)
	include(CMakePackageConfigHelpers)
	include(GNUInstallDirs)
endif()

add_subdirectory(engine)
add_subdirectory(editor)
