/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef NOVA_DX12

#include "drivers/dx12/render_driver.h"

#include <nova/core/debug.h>

using namespace Nova;

DX12RenderDriver::DX12RenderDriver() {
	NOVA_AUTO_TRACE();
}

DX12RenderDriver::~DX12RenderDriver() {
	NOVA_AUTO_TRACE();
}

#endif // NOVA_DX12
