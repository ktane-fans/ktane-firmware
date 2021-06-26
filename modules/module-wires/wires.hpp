#pragma once

#include <array>

#include "shared/numbers.hpp"

using std::array;

constexpr size_t MAX_WIRE_COUNT = 6;

enum class WireColor
{
	NONE,
	RED,
	WHITE,
	BLUE,
	YELLOW,
	BLACK,
};

byte count_color(const array<WireColor, MAX_WIRE_COUNT> wire_state, const WireColor color);
byte count_wires(const array<WireColor, MAX_WIRE_COUNT> wire_state);
size_t get_index_when_ignoring_none(const array<WireColor, MAX_WIRE_COUNT> wire_state, const size_t index);
size_t get_last_index_of_color(const array<WireColor, MAX_WIRE_COUNT> wire_state, const WireColor color);
size_t get_index_to_cut(const array<WireColor, MAX_WIRE_COUNT> wire_state, const bool last_digit_is_odd);
