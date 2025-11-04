/**
 * Copyright (c) 2025, Jayden Grubb <contact@jaydengrubb.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

namespace Nova::Utils {

template<typename... Ts>
struct Overloaded : Ts... {
	using Ts::operator()...;
};

template<typename... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

} // namespace Nova::Utils
