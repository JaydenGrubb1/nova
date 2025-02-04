# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

set(SRC
	test.cpp
)

list(TRANSFORM SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${Vulkan_INCLUDE_DIRS}
)

if (NOVA_ENGINE_SHARED)
	add_library(nova SHARED ${SRC})
	target_compile_definitions(nova PRIVATE
		NOVA_DLL_EXPORT
	)
endif ()
if (NOVA_ENGINE_STATIC)
	add_library(nova_static STATIC ${SRC})
endif ()
