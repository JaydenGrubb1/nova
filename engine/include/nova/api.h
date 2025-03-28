/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifndef NOVA_API
	#ifdef NOVA_WINDOWS
		#ifdef NOVA_DLL_EXPORT
			#define NOVA_API __declspec(dllexport)
		#elif NOVA_DLL_IMPORT
			#define NOVA_API __declspec(dllimport)
		#else
			#define NOVA_API
		#endif
	#elif NOVA_LINUX
		#ifdef NOVA_DLL_EXPORT
			#define NOVA_API __attribute__((visibility("default")))
		#else
			#define NOVA_API
		#endif
	#endif
#endif
