/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#ifdef NOVA_DX12

#include <nova/render/render_driver.h>

namespace Nova {
	class DX12RenderDriver final : public RenderDriver {
	  public:
		DX12RenderDriver();
		~DX12RenderDriver() override;
	};
} // namespace Nova

#endif // NOVA_DX12
