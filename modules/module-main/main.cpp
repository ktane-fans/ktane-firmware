#include "main.hpp"

CANMessageBus bus;
Buzzer speaker(A4);
SevenSegmentArray<4> time_display(A0, A1, A2);
Diode countdown_second_indicator(D7);
Diode strike_diodes[] = {
	Diode(D3),
	Diode(D4),
};

byte module_count = 0;
byte solved_modules = 0;
Strikes strikes = 0;

constexpr Strikes max_strikes = 3;
constexpr bool has_vowels = true;
constexpr bool last_digit_is_odd = true;

Duration initial_time_remaining = 5_min;
constexpr Duration time_for_each_module = 1_min;
constexpr Duration base_remaining_time = 2_min;

constexpr Duration wait_for_setup_time = 5_s;
constexpr Duration wait_for_game_start_time = 10_s;
constexpr Duration max_remaining_time_for_acoustic_countdown = 15_s;
constexpr Duration game_lost_sound_duration = 2_s;
constexpr Duration second_indicator_flash_duration = 100_ms;
constexpr Duration time_where_to_display_centi_and_deci_seconds = 60_s;

Timestamp game_started_at = 0_s;

enum class State : byte
{
	// Game init states
	WAIT_FOR_SETUP,
	WAIT_FOR_START,

	RUNNING,

	// General Module states
	LOST,
	WON,
};

State state = State::WAIT_FOR_SETUP;

// Do networking in different thread so the setup method gets called immediately.
SYSTEM_THREAD(ENABLED);

void setup()
{
	RGB.control(true);
	RGB.brightness(255);

	Random::init();

	for (auto &led : strike_diodes)
		led.init();

	countdown_second_indicator.init();

	display_strike_status(0, max_strikes);

	time_display.init();
	time_display.clear();
	time_display.displaySegments({SegmentChar::I, SegmentChar::A, SegmentChar::H, SegmentChar::O});

	bus.init();

	bus.on(GameMessageType::MODULE_SOLVED, &module_solved);
	bus.on(GameMessageType::MODULE_FAILED, &module_failed);
	bus.on(GameMessageType::MODULE_REGISTER, &module_register);
}

void loop()
{
	bus.tick();

	static Timestamp setup_wait_start = 0;
	static Duration previous_total_seconds = -1;
	static Timestamp second_announced_at = 0;
	static Duration time_remaining = initial_time_remaining;

	switch (state)
	{
	case State::WAIT_FOR_SETUP:
	{
		if (setup_wait_start == 0)
		{
			setup_wait_start = millis();
			return;
		}

		const auto now = millis();
		bool should_send_setup = (now - setup_wait_start) >= wait_for_setup_time;
		if (should_send_setup)
		{
			setup_wait_start = 0;
			state = State::WAIT_FOR_START;
			speaker.beep(100);
			bus.send(GameSetupMessage(strikes, has_vowels, last_digit_is_odd));
		}
		return;
	}
	case State::WAIT_FOR_START:
	{
		if (setup_wait_start == 0)
		{
			setup_wait_start = millis();
			return;
		}

		const auto now = millis();
		bool should_start = (now - setup_wait_start) >= wait_for_game_start_time;
		if (should_start)
		{
			setup_wait_start = 0;
			game_started_at = millis();

			initial_time_remaining = base_remaining_time + time_for_each_module * module_count;

			time_remaining = initial_time_remaining;
			state = State::RUNNING;
			speaker.beepShort(100);
			bus.send(GameStartMessage());
		}
		return;
	}
	case State::RUNNING:
	{
		const auto now = millis();
		auto passed_time = now - game_started_at;
		if (passed_time > initial_time_remaining)
		{
			game_lost();
			return;
		}

		time_remaining = initial_time_remaining - passed_time;
		auto time_to_display = time_remaining < time_where_to_display_centi_and_deci_seconds
								   ? get_display_time_ss_dc(time_remaining)
								   : get_display_time_mm_ss(time_remaining);

		time_display.display(time_to_display.time_as_integer);

		// if the "total seconds" have changed (which is an integer), we entered a new second.
		bool entered_a_new_second = previous_total_seconds != time_to_display.total_seconds;

		if (entered_a_new_second)
		{
			countdown_second_indicator.on();
			second_announced_at = now;

			// we entered a new second and we are in the interval where we have to beep
			if (time_remaining <= max_remaining_time_for_acoustic_countdown)
				speaker.on_timed(100);
		}
		else
		{
			// we have already seen this second
			auto time_passed_since_second_was_announced = now - second_announced_at;

			if (time_passed_since_second_was_announced >= second_indicator_flash_duration)
			{
				// Also, at least "second_indicator_flash_duration" has passed
				// -> we need to turn off the indicator again
				countdown_second_indicator.off();
				second_announced_at = 0;
			}
		}
		previous_total_seconds = time_to_display.total_seconds;
		return;
	}
	case State::LOST:
		RGB.color(RGBColor::Red);
		return;
	case State::WON:
		RGB.color(RGBColor::Green);
		return;
	default:
		return;
	}
}

void game_lost()
{
	state = State::LOST;

	time_display.displaySegments({SegmentChar::B, SegmentChar::O, SegmentChar::O, SegmentChar::N});
	countdown_second_indicator.off();

	speaker.on_timed(game_lost_sound_duration);
	bus.send(GameLostMessage());
}

/**
 * Returns the display tine in Minutes and Seconds.
 */
const DisplayTime get_display_time_mm_ss(const Duration d)
{
	auto total_seconds = d / 1_s;
	auto minutes = total_seconds / 60;
	auto seconds = total_seconds % 60;
	return {
		minutes * 100 + seconds,
		total_seconds,
	};
}

/**
 * Returns the display tine in Seconds and Deci/Centiseconds.
 */
const DisplayTime get_display_time_ss_dc(const Duration d)
{
	const auto total_seconds = d / 1_s;
	const auto seconds_centi_deci_milli = likely(d < 60_s)
											  ? d
											  : d % 60_s;
	const auto seconds_centi_deci = seconds_centi_deci_milli / 10;
	return {
		seconds_centi_deci,
		total_seconds,
	};

	// Old approach, should be slower due to more integer modulos etc.:
	// TODO: Remove as soon as the new one seems to work
	/*
	const auto total_seconds = d / 1000;
	const auto seconds = total_seconds % 60;
	const auto deci_centi_milliseconds_above_whole_seconds = d % 1000;
	const auto deci_and_centi_seconds = (deci_centi_milliseconds_above_whole_seconds) / 10;
	return {
		seconds * 100 + deci_and_centi_seconds,
		total_seconds,
	};
	*/
}

void module_solved(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const ModuleSolvedMessage &>(m);

	if (state != State::RUNNING)
		return;

	++solved_modules;
	speaker.beepShort();

	if (solved_modules == module_count)
	{
		state = State::WON;
	}
}
void module_failed(const GameMessage &m)
{
	if (state != State::RUNNING)
		return;

	++strikes;

	display_strike_status(strikes, max_strikes);

	if (strikes >= max_strikes)
	{
		game_lost();
	}
	else
	{
		speaker.on_timed(500, 330);
		bus.send(GameUpdateMessage(strikes));
	}
}
void module_register(const GameMessage &m)
{
	if (state != State::WAIT_FOR_START)
		return;

	++module_count;

	time_display.display(module_count);
}

void display_strike_status(const Strikes strikes, const Strikes max_strikes)
{
	switch (strikes)
	{
	case 2:
		strike_diodes[0].on();
		strike_diodes[1].on();
		break;
	case 1:
		strike_diodes[0].on();
		strike_diodes[1].off();
		break;
	default:
		strike_diodes[0].off();
		strike_diodes[1].off();
		break;
	}
}
