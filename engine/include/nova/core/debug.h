/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/api.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace Nova {
	namespace Internals {
		NOVA_API void _assert_fail(std::string_view assertion, std::string_view file, std::string_view func, int line);

		consteval std::string_view _format_func_name(const std::string_view name) {
			const auto end = name.find('(');
			const auto start = name.rfind(' ', end) + 1;
			return name.substr(start, end - start);
		}
	} // namespace Internals

	class NOVA_API Debug {
	  public:
		static std::shared_ptr<spdlog::logger> get_logger();
		static bool is_debug();

		template<typename... Args>
		static void log(spdlog::format_string_t<Args...> fmt, Args&&... args) {
			get_logger()->info(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void log_warning(spdlog::format_string_t<Args...> fmt, Args&&... args) {
			get_logger()->warn(fmt, std::forward<Args>(args)...);
		}

		template<typename... Args>
		static void log_error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
			get_logger()->error(fmt, std::forward<Args>(args)...);
		}
	};
} // namespace Nova

#define NOVA_LOG(...) ::Nova::Debug::get_logger()->info(__VA_ARGS__)
#define NOVA_WARN(...) ::Nova::Debug::get_logger()->warn(__VA_ARGS__)
#define NOVA_ERROR(...) ::Nova::Debug::get_logger()->error(__VA_ARGS__)
#define NOVA_CRITICAL(...) ::Nova::Debug::get_logger()->critical(__VA_ARGS__)
#define NOVA_DEBUG(...) ::Nova::Debug::get_logger()->debug(__VA_ARGS__)
#define NOVA_TRACE(...) ::Nova::Debug::get_logger()->trace(__VA_ARGS__)

#ifdef _MSC_VER
#define NOVA_FUNC_NAME ::Nova::Internals::_format_func_name(__FUNCTION__)
#else
#define NOVA_FUNC_NAME ::Nova::Internals::_format_func_name(__PRETTY_FUNCTION__)
#endif

#define NOVA_AUTO_TRACE() NOVA_TRACE("{}()", NOVA_FUNC_NAME)

#define NOVA_ASSERT(expr) \
	(static_cast<bool>(expr) \
		 ? static_cast<void>(0) \
		 : ::Nova::Internals::_assert_fail(#expr, __FILE__, NOVA_FUNC_NAME, __LINE__))
