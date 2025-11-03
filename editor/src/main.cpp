/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdlib>
#include <print>

int main(int argc, char* argv[]) {
	std::println("Hello, World!");

	for (int i = 0; i < argc; i++) {
		std::println("argv[{}]: {}", i, argv[i]);
	}

	return EXIT_SUCCESS;
}
