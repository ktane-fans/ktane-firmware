#pragma once

#include <Particle.h>
#include <map>
#include <array>

#include "shared/bus/communication.hpp"
#include "shared/GameState.hpp"
#include "shared/Diode.hpp"
#include "shared/Button.hpp"
#include "shared/Random.hpp"
#include "shared/OhmMeter.hpp"
#include "shared/ModuleStatusDiode.hpp"

#include "wires.hpp"

using std::array;

void setup_handler(const GameMessage &m);
void start_handler(const GameMessage &m);
void update_handler(const GameMessage &m);
void lost_handler(const GameMessage &m);

enum class State : byte
{
	// Game init states
	WAIT_FOR_SETUP,
	WAIT_FOR_START,

	WAIT_FOR_CUT,

	// General Module states
	SOLVED,
	FINISHED,
	STRIKE,
	WAIT_FOR_STRIKE_UPDATE,
};

class WireDetector
{
private:
	const int NUM_MEASURES = 5;
	const OhmMeter meter;
	const std::map<Resistance, WireColor> resistance_map = {
		{56, WireColor::RED},
		{150, WireColor::WHITE},
		{270, WireColor::BLUE},
		{470, WireColor::YELLOW},
		{1000, WireColor::BLACK},
	};

public:
	WireDetector(const AnalogPin pin, const Resistance reference_resistor)
		: meter(OhmMeter(pin, reference_resistor)) {}

	WireColor read_wire_color() const
	{
		Resistance resistance = 0;
		for (int i = 0; i < NUM_MEASURES; ++i)
		{
			resistance += meter.measure();
			if (i == 0 && resistance > 9000)
				return WireColor::NONE;
		}
		resistance /= NUM_MEASURES;

		if (resistance > 9000)
			return WireColor::NONE;

		Resistance current_minimum_delta = 9000;
		WireColor current_color = WireColor::NONE;
		for (auto &entry : resistance_map)
		{
			Resistance delta = abs(entry.first - resistance);
			if (delta < current_minimum_delta)
			{
				current_minimum_delta = delta;
				current_color = entry.second;
			}
		}
		return current_color;
	}
};
