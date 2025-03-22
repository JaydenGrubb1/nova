/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <nova/render/render_driver.h>

namespace Nova {
	class DX12RenderDriver final : public RenderDriver {
	  public:
		DX12RenderDriver();
		~DX12RenderDriver() override;

		[[nodiscard]] RenderAPI get_api() const override;
		[[nodiscard]] u32 get_api_version() const override;
		[[nodiscard]] std::string get_api_name() const override;
		[[nodiscard]] std::string get_api_version_string() const override;

		[[nodiscard]] u32 get_device_count() const override;
		[[nodiscard]] const RenderDevice& get_device(u32 index) const override;
		void create_device(u32 index) override;
	};
} // namespace Nova
