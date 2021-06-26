#include "wires.hpp"

byte count_color(const array<WireColor, MAX_WIRE_COUNT> wire_state, const WireColor color)
{
	byte count = 0;
	for (size_t i = 0; i < MAX_WIRE_COUNT; ++i)
	{
		if (wire_state[i] == color)
			++count;
	}
	return count;
}

byte count_wires(const array<WireColor, MAX_WIRE_COUNT> wire_state)
{
	byte count = 0;
	for (size_t i = 0; i < MAX_WIRE_COUNT; ++i)
	{
		if (wire_state[i] != WireColor::NONE)
			++count;
	}
	return count;
}

size_t get_index_when_ignoring_none(const array<WireColor, MAX_WIRE_COUNT> wire_state, const size_t index)
{
	size_t res = 0;
	for (size_t i = 0; i < MAX_WIRE_COUNT; ++i)
	{
		if (wire_state[i] != WireColor::NONE)
			++res;
		if (res > index)
			return i;
	}
	// TODO: Handle the case of having no
	return -1;
}
size_t get_last_index_of_color(const array<WireColor, MAX_WIRE_COUNT> wire_state, const WireColor color)
{
	size_t last_index = -1;
	for (size_t i = 0; i < MAX_WIRE_COUNT; ++i)
	{
		if (wire_state[i] == color)
			last_index = i;
	}
	return last_index;
}

size_t get_index_to_cut(const array<WireColor, MAX_WIRE_COUNT> wire_state, const bool last_digit_is_odd)
{
	constexpr size_t first = 0;
	switch (count_wires(wire_state))
	{
	case 3:
	{
		constexpr size_t last = 2;

		if (count_color(wire_state, WireColor::RED) == 0)
		{
			return get_index_when_ignoring_none(wire_state, 1);
		}
		else
		{
			auto const last_wire_index = get_index_when_ignoring_none(wire_state, last);

			if (wire_state[last_wire_index] == WireColor::WHITE)
			{
				return last_wire_index;
			}
			else if (count_color(wire_state, WireColor::BLUE) > 1)
			{
				return get_last_index_of_color(wire_state, WireColor::BLUE);
			}
			else
			{
				return get_index_when_ignoring_none(wire_state, last);
			}
		}
	}
	case 4:
	{
		constexpr size_t last = 3;

		if (count_color(wire_state, WireColor::RED) > 1 && last_digit_is_odd)
		{
			return get_last_index_of_color(wire_state, WireColor::RED);
		}
		else
		{
			auto const last_wire_index = get_index_when_ignoring_none(wire_state, last);

			if (wire_state[last_wire_index] == WireColor::YELLOW && count_color(wire_state, WireColor::RED) == 0)
			{
				return get_index_when_ignoring_none(wire_state, first);
			}
			else if (count_color(wire_state, WireColor::BLUE) == 1)
			{
				return get_index_when_ignoring_none(wire_state, first);
			}
			else if (count_color(wire_state, WireColor::YELLOW) > 1)
			{
				return get_index_when_ignoring_none(wire_state, last);
			}
			else
			{
				return get_index_when_ignoring_none(wire_state, 1);
			}
		}
	}
	case 5:
	{
		constexpr size_t last = 4;

		auto const last_wire_index = get_index_when_ignoring_none(wire_state, last);
		if (last_digit_is_odd && wire_state[last_wire_index] == WireColor::BLACK)
		{
			return get_index_when_ignoring_none(wire_state, 3);
		}
		else if (count_color(wire_state, WireColor::RED) == 1 && count_color(wire_state, WireColor::YELLOW) > 1)
		{
			return get_index_when_ignoring_none(wire_state, first);
		}
		else if (count_color(wire_state, WireColor::BLACK) == 0)
		{
			return get_index_when_ignoring_none(wire_state, 1);
		}
		else
		{
			return get_index_when_ignoring_none(wire_state, first);
		}
	}
	case 6:
	{
		const size_t last = 5;

		if (last_digit_is_odd && count_color(wire_state, WireColor::YELLOW) == 0)
		{
			return get_index_when_ignoring_none(wire_state, 2);
		}
		else if (count_color(wire_state, WireColor::YELLOW) == 1 && count_color(wire_state, WireColor::WHITE) > 1)
		{
			return get_index_when_ignoring_none(wire_state, 3);
		}
		else if (count_color(wire_state, WireColor::RED) == 0)
		{
			return get_index_when_ignoring_none(wire_state, last);
		}
		else
		{
			return get_index_when_ignoring_none(wire_state, 3);
		}
	}
	default:
		// Return MAX_WIRE_COUNT because we did not see the supported amount of wires
		return MAX_WIRE_COUNT;
	}
}
