# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

set(SRC
	main.cpp
)

list(TRANSFORM SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_executable(nova-editor ${SRC})

target_include_directories(nova-editor PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/engine/include
)

if (NOVA_EDITOR_STATIC)
	target_link_libraries(nova-editor PRIVATE
		nova_static
	)
else ()
	target_link_libraries(nova-editor PRIVATE
		nova
	)
	target_compile_definitions(nova-editor PRIVATE
		NOVA_DLL_IMPORT
	)
	if (CMAKE_IMPORT_LIBRARY_SUFFIX)
		add_custom_command(TARGET nova-editor POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:nova-editor> $<TARGET_FILE_DIR:nova-editor>
			COMMAND_EXPAND_LISTS
		)
	endif ()
endif ()
