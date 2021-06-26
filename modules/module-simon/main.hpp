#pragma once

#include <Particle.h>

#include "shared/bus/communication.hpp"
#include "shared/GameState.hpp"
#include "shared/Diode.hpp"
#include "shared/Button.hpp"
#include "shared/Buzzer.hpp"
#include "shared/Random.hpp"
#include "shared/ModuleStatusDiode.hpp"
#include "shared/units/time.hpp"

void setup_handler(const GameMessage &m);
void start_handler(const GameMessage &m);
void update_handler(const GameMessage &m);
void lost_handler(const GameMessage &m);

/** Colors that correspond to the LED/Button arrays */
enum class SimonColor : byte
{
	Red = 0,
	Blue,
	Green,
	Yellow,
};

const Frequency frequencies[] = {
	/** Red */
	659_Hz,
	/** Blue */
	987_Hz,
	/** Green */
	783_Hz,
	/** Yellow */
	1108_Hz,
};

const SimonColor LAST_COLOR = SimonColor::Yellow;

SimonColor random_color()
{
	return (SimonColor)random(0, (byte)LAST_COLOR + 1 /* last color + 1 */);
}

enum class State : byte
{
	// Game init states
	WAIT_FOR_SETUP,
	WAIT_FOR_START,

	// Module-Specific States
	COLOR_FLASHING,
	WAIT_FOR_INPUT,

	// General Module states
	SOLVED,
	FINISHED,
	STRIKE,
	WAIT_FOR_STRIKE_UPDATE,
};

void initialize_expected_colors(SimonColor colors[]);

void flash_single_color(const SimonColor color);
bool flash_colors(const SimonColor colors[], const size_t to);
void turn_all_colors_off();

SimonColor manual_color_mapping_function(const bool has_vowel, const Strikes strikes, const SimonColor flashed_color);
SimonColor identity_color_mapping_function(const bool has_vowel, const Strikes strikes, const SimonColor flashed_color);
typedef decltype(&manual_color_mapping_function) MappingFunction;
