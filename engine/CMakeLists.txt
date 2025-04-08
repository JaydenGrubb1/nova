# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

include(FetchContent)

set(NOVA_DX12 ON CACHE BOOL "Enable DirectX 12 support")
set(NOVA_VULKAN ON CACHE BOOL "Enable Vulkan support")
set(NOVA_WAYLAND ON CACHE BOOL "Enable Wayland support")
set(NOVA_X11 ON CACHE BOOL "Enable X11 support")

if (WIN32)
	set(NOVA_WAYLAND OFF)
	set(NOVA_X11 OFF)
else ()
	set(NOVA_DX12 OFF)
endif ()

if (NOVA_VULKAN)
	find_package(Vulkan REQUIRED)
endif ()

set(ENGINE_SRC
	core/debug.cpp
	drivers/dx12/render_driver.cpp
	drivers/vulkan/render_driver.cpp
	drivers/wayland/window_driver.cpp
	drivers/win32/window_driver.cpp
	drivers/x11/window_driver.cpp
	platform/window_driver.cpp
	render/renderer.cpp
	render/render_device.cpp
	render/render_driver.cpp
)

list(TRANSFORM ENGINE_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY https://github.com/gabime/spdlog.git
	GIT_TAG f355b3d58f7067eee1706ff3c801c2361011f3d5 # v1.15.1
	FIND_PACKAGE_ARGS NAMES spdlog
)
FetchContent_MakeAvailable(spdlog)

set(ENGINE_INCLUDES_PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	$<$<BOOL:${NOVA_VULKAN}>:${Vulkan_INCLUDE_DIRS}>
)
set(ENGINE_INCLUDES_PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${spdlog_INCLUDE_DIRS}
)
set(ENGINE_LIBS_PRIVATE
	$<$<BOOL:${NOVA_VULKAN}>:Vulkan::Vulkan>
	$<$<BOOL:${NOVA_X11}>:X11>
)
set(ENGINE_LIBS_PUBLIC
	spdlog::spdlog
)
set(ENGINE_DEFS
	$<$<BOOL:${NOVA_DX12}>:NOVA_DX12>
	$<$<BOOL:${NOVA_VULKAN}>:NOVA_VULKAN>
	$<$<BOOL:${NOVA_WAYLAND}>:NOVA_WAYLAND>
	$<$<BOOL:${NOVA_X11}>:NOVA_X11>
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
		${ENGINE_DEFS}
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
	target_compile_definitions(nova_static PRIVATE
		${ENGINE_DEFS}
	)
endif ()
