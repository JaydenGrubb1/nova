/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <nova/core/debug.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace Nova;

std::shared_ptr<spdlog::logger> Debug::get_logger() {
	static std::shared_ptr<spdlog::logger> s_logger;
	if (!s_logger) {
		s_logger = spdlog::stdout_color_mt("NOVA");
		s_logger->set_pattern("%^[%T] %n: %v%$");
	}
	return s_logger;
}

bool Debug::is_debug() {
#ifdef NDEBUG
	return false;
#else
	return true;
#endif
}

void Internals::_assert_fail(std::string_view p_assertion, std::string_view p_file, std::string_view p_func, int p_line) {
	Debug::get_logger()->critical(
		"ASSERTION FAILED\n Assertion: \"{}\"\n File:      \"{}\" (line {})\n Function:  {}()",
		p_assertion,
		p_file,
		p_line,
		p_func
	);
	std::abort();
}
