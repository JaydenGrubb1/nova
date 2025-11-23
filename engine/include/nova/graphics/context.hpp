/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/core/api.hpp>
#include <nova/core/types.hpp>

#include <memory>
#include <string>

namespace nova::gfx {

class IDevice;

enum class API {
	UNDEFINED,
	VULKAN,
};

class NOVA_API IContext {
  public:
	static std::unique_ptr<IContext> create(API api);
	virtual ~IContext() = default;

	virtual API get_api() const = 0;
	virtual std::string get_api_name() const = 0;

	virtual u32 get_api_version() const = 0;
	virtual std::string get_api_version_string() const = 0;

	virtual std::unique_ptr<IDevice> create_device() = 0;
};

} // namespace nova::gfx
