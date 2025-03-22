/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "drivers/dx12/render_driver.h"

#include <nova/core/debug.h>

using namespace Nova;

DX12RenderDriver::DX12RenderDriver() {
	NOVA_AUTO_TRACE();
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
}

DX12RenderDriver::~DX12RenderDriver() {
	NOVA_AUTO_TRACE();
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
}

RenderAPI DX12RenderDriver::get_api() const {
	return RenderAPI::DX12;
}

u32 DX12RenderDriver::get_api_version() const {
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
	return 0;
}

std::string DX12RenderDriver::get_api_name() const {
	return "DX12";
}

std::string DX12RenderDriver::get_api_version_string() const {
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
	return "";
}

u32 DX12RenderDriver::get_device_count() const {
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
	return 0;
}

const RenderDevice& DX12RenderDriver::get_device(u32 index) const {
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
	static RenderDevice device {
		.name = "UNKNOWN",
		.vendor = RenderDevice::Vendor::UNKNOWN,
		.type = RenderDevice::Type::OTHER,
		.deviceID = 0,
		.handle = nullptr
	};
	(void)index;
	return device;
}

void DX12RenderDriver::create_device(u32 index) {
	NOVA_AUTO_TRACE();
	NOVA_WARN("{}() not implemented", NOVA_FUNC_NAME);
	(void)index;
}
