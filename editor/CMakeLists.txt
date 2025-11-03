# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

set(NOVA_EDITOR_SRC
	main.cpp
)

list(TRANSFORM NOVA_EDITOR_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_executable(nova-editor ${NOVA_EDITOR_SRC})

target_include_directories(nova-editor
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(nova-editor
	PUBLIC nova
)
