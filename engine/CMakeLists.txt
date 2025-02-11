# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Vulkan REQUIRED)
include(FetchContent)

set(ENGINE_SRC
	drivers/vulkan/render_driver.cpp
	render/renderer.cpp
)

list(TRANSFORM ENGINE_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${Vulkan_INCLUDE_DIRS}
)

set(ENGINE_LIBS
	Vulkan::Vulkan
)

if (NOVA_ENGINE_SHARED)
	add_library(nova SHARED ${ENGINE_SRC})
	target_compile_definitions(nova PRIVATE
		NOVA_DLL_EXPORT
	)
	target_link_libraries(nova PRIVATE
		${ENGINE_LIBS}
	)
endif ()
if (NOVA_ENGINE_STATIC)
	add_library(nova_static STATIC ${ENGINE_SRC})
	target_link_libraries(nova_static PRIVATE
		${ENGINE_LIBS}
	)
endif ()
