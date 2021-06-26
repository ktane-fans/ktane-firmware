#pragma once

#include <Particle.h>
#include <neopixel.h>

#include "shared/bus/communication.hpp"
#include "shared/Button.hpp"
#include "shared/Buzzer.hpp"
#include "shared/Diode.hpp"
#include "shared/Random.hpp"
#include "shared/numbers.hpp"

#include "shared/output/SevenSegmentArray.hpp"

struct DisplayTime
{
	/**
	 * The time remaining.
	 * - 600 seconds (10 minutes) will be 1000
	 * - 660 seconds (11 minutes) will be 1100
	 * - 665 seconds (11 minutes and 5 seconds) will be 1105
	 * - 695 seconds (11 minutes and 35 seconds) will be 1135
	 */
	u32 time_as_integer;

	/**
	 * Not using "Duration" as type because that would indicate it would be milliseconds.
	 *
	 * However, we need integer seconds!
	 */
	u32 total_seconds;
};

/**
 * Returns the display tine in Minutes and Seconds.
 */
const DisplayTime get_display_time_mm_ss(const Duration d);

/**
 * Returns the display tine in Seconds and Deci/Centiseconds.
 */
const DisplayTime get_display_time_ss_dc(const Duration d);

void display_strike_status(const Strikes strikes, const Strikes max_strikes);

void game_lost();

void module_solved(const GameMessage &m);
void module_register(const GameMessage &m);
void module_failed(const GameMessage &m);

void send_setup();
