/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

// clang-format off

#if !defined(NOVA_API)
	#if defined(NOVA_PLATFORM_WINDOWS)
		#if defined(NOVA_EXPORT_SYMBOLS)
			#define NOVA_API __declspec(dllexport)
		#else
			#define NOVA_API __declspec(dllimport)
		#endif
	#elif defined(NOVA_PLATFORM_MACOS) || defined(NOVA_PLATFORM_LINUX)
		#if defined(NOVA_EXPORT_SYMBOLS)
			#define NOVA_API __attribute__((visibility("default")))
		#else
			#define NOVA_API
		#endif
	#else
		#warning "NOVA_API is not defined for this platform"
		#define NOVA_API
	#endif
#endif

// clang-format on
