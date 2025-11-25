# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

include(FetchContent)

set(NOVA_ENGINE_SRC
	core/debug.cpp
	graphics/context.cpp
)

list(TRANSFORM NOVA_ENGINE_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_library(nova ${NOVA_ENGINE_SRC})

target_include_directories(nova
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (NOVA_BACKEND_VULKAN)
	find_package(Vulkan REQUIRED)

	set(NOVA_VULKAN_SRC
		backends/vulkan/context.cpp
		backends/vulkan/device.cpp
	)

	list(TRANSFORM NOVA_VULKAN_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

	target_sources(nova PRIVATE ${NOVA_VULKAN_SRC})
	target_compile_definitions(nova PRIVATE NOVA_BACKEND_VULKAN)
	target_include_directories(nova PRIVATE ${Vulkan_INCLUDE_DIRS})

	if (NOVA_USE_VOLK)
		FetchContent_Declare(
			volk
			GIT_REPOSITORY https://github.com/zeux/volk.git
			GIT_TAG f30088b3f4160810b53e19258dd2f7395e5f0ba3 # 1.4.328.1
		)
		FetchContent_MakeAvailable(volk)

		target_compile_definitions(nova PRIVATE NOVA_USE_VOLK)
		target_include_directories(nova PRIVATE ${volk_INCLUDE_DIRS})
		target_link_libraries(nova PRIVATE volk::volk)
	else()
		target_link_libraries(nova PRIVATE Vulkan::Vulkan)
	endif()
endif()

set_target_properties(nova PROPERTIES
	DEFINE_SYMBOL NOVA_EXPORT_SYMBOLS
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION_MAJOR}
)

add_library(nova::nova ALIAS nova)

if (NOVA_LIBRARY_INSTALL)
	configure_package_config_file(
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/nova-config.cmake.in
		${CMAKE_CURRENT_BINARY_DIR}/nova-config.cmake
		INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nova
	)

	write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/nova-config-version.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion
	)

	install(
		TARGETS nova
		EXPORT nova-targets
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)

	install(
		DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/nova
		DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
		FILES_MATCHING PATTERN "*.hpp"
	)

	install(
		EXPORT nova-targets
		FILE nova-targets.cmake
		NAMESPACE nova::
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nova
	)

	install(
		FILES
			${CMAKE_CURRENT_BINARY_DIR}/nova-config.cmake
			${CMAKE_CURRENT_BINARY_DIR}/nova-config-version.cmake
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nova
	)
endif()
