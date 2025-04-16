/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

// clang-format off

#ifdef NOVA_COMPILER_GCC
	#define NOVA_COMPILER_DIAGNOSTIC_POP									\
		_Pragma("GCC diagnostic pop")
#elif NOVA_COMPILER_CLANG
	#define NOVA_COMPILER_DIAGNOSTIC_POP									\
		_Pragma("clang diagnostic pop")
#elif NOVA_COMPILER_MSVC
	#define NOVA_COMPILER_DIAGNOSTIC_POP									\
		__pragma(warning(pop))
#else
	#define NOVA_COMPILER_DIAGNOSTIC_POP
#endif

#ifdef NOVA_COMPILER_GCC
	#define NOVA_BEGIN_ALLOW_ANONYMOUS_TYPES								\
		_Pragma("GCC diagnostic push")										\
		_Pragma("GCC diagnostic ignored \"-Wpedantic\"")
#elif NOVA_COMPILER_CLANG
	#define NOVA_BEGIN_ALLOW_ANONYMOUS_TYPES								\
		_Pragma("clang diagnostic push")									\
		_Pragma("clang diagnostic ignored \"-Wgnu-anonymous-struct\"")		\
		_Pragma("clang diagnostic ignored \"-Wnested-anon-types\"")
#elif NOVA_COMPILER_MSVC
	#define NOVA_BEGIN_ALLOW_ANONYMOUS_TYPES								\
		__pragma(warning(push))												\
		__pragma(warning(disable : 4201))
#else
	#define NOVA_BEGIN_ALLOW_ANONYMOUS_TYPES
#endif

#define NOVA_END_ALLOW_ANONYMOUS_TYPES										\
	NOVA_COMPILER_DIAGNOSTIC_POP

// clang-format on
