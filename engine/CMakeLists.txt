# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Vulkan REQUIRED)
include(FetchContent)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG f355b3d58f7067eee1706ff3c801c2361011f3d5 # v1.15.1
	FIND_PACKAGE_ARGS NAMES spdlog
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG 22422f7748d5128135995ed34c8f8012861c7332 # preview-3.1.8
	FIND_PACKAGE_ARGS NAMES SDL3
)
FetchContent_MakeAvailable(SDL3)

set(ENGINE_SRC
	core/debug.cpp
	drivers/vulkan/render_driver.cpp
	drivers/windows/system_driver.cpp
	drivers/linux/system_driver.cpp
	drivers/linux/wayland/system_driver.cpp
	drivers/linux/x11/system_driver.cpp
	platform/system.cpp
	render/renderer.cpp
	render/render_device.cpp
)

list(TRANSFORM ENGINE_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

set(ENGINE_INCLUDES_PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${Vulkan_INCLUDE_DIRS}
	${SDL3_INCLUDE_DIRS}
)
set(ENGINE_INCLUDES_PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${spdlog_INCLUDE_DIRS}
)
set(ENGINE_LIBS_PRIVATE
	Vulkan::Vulkan
	SDL3::SDL3
)
set(ENGINE_LIBS_PUBLIC
	spdlog::spdlog
)

if (NOVA_ENGINE_SHARED)
	add_library(nova SHARED ${ENGINE_SRC})
	target_include_directories(nova
		PUBLIC ${ENGINE_INCLUDES_PUBLIC}
		PRIVATE ${ENGINE_INCLUDES_PRIVATE}
	)
	target_link_libraries(nova
		PUBLIC ${ENGINE_LIBS_PUBLIC}
		PRIVATE ${ENGINE_LIBS_PRIVATE}
	)
	target_compile_definitions(nova PRIVATE
		NOVA_DLL_EXPORT
	)
endif ()
if (NOVA_ENGINE_STATIC)
	add_library(nova_static STATIC ${ENGINE_SRC})
	target_include_directories(nova_static PUBLIC
		PUBLIC ${ENGINE_INCLUDES_PUBLIC}
		PRIVATE ${ENGINE_INCLUDES_PRIVATE}
	)
	target_link_libraries(nova_static PUBLIC
		PUBLIC ${ENGINE_LIBS_PUBLIC}
		PRIVATE ${ENGINE_LIBS_PRIVATE}
	)
endif ()
