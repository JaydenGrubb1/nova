# Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
# SPDX-License-Identifier: BSD-3-Clause

set(NOVA_ENGINE_SRC
	core/debug.cpp
)

list(TRANSFORM NOVA_ENGINE_SRC PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)

add_library(nova SHARED ${NOVA_ENGINE_SRC})

target_include_directories(nova
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(nova
	PRIVATE NOVA_EXPORT_SYMBOLS
)
