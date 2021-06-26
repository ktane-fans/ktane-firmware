#include "main.hpp"

CANMessageBus bus;
ModuleStatusDiode module_status_led(A5, D0);
Buzzer sound(D3);

GameState global_game_state{
	.lost = false,
	.strikes = 0,
	.has_vowels = false,
	.last_digit_is_odd = false,
};

Diode leds[] = {
	Diode(A0),
	Diode(A1),
	Diode(A2),
	Diode(A3),
};

Button buttons[] = {
	Button(D4),
	Button(D5),
	Button(D6),
	Button(D7),
};

SimonColor colors[] = {
	SimonColor::Red,
	SimonColor::Red,
	SimonColor::Red,
	SimonColor::Red,
};

void initialize_colors()
{
	for (auto &color : colors)
		color = random_color(); // does only work when we use "auto&"
}

const Duration retry_timeout = 10_s;

// Do networking in different thread so the setup method gets called immediately.
SYSTEM_THREAD(ENABLED);

void setup()
{
	RGB.control(true);

	bus.init();

	Random::init();

	module_status_led.init();
	module_status_led.off();

	sound.init();

	initialize_colors();

	for (auto &l : leds)
		l.init();

	for (auto &b : buttons)
		b.init();

	bus.on(GameMessageType::GAME_SETUP, &setup_handler);
	bus.on(GameMessageType::GAME_START, &start_handler);
	bus.on(GameMessageType::GAME_UPDATE, &update_handler);
	bus.on(GameMessageType::GAME_LOST, &lost_handler);
}

State state = State::WAIT_FOR_SETUP;

/** current expected color index */
int expected_color_index = 0;
/** current target index */
int required = 0;
/** end index (exclusive) */
const int end = sizeof(colors) / sizeof(colors[0]);

const system_tick_t delay_between_start_and_first_flash = 2 * 1000;

MappingFunction get_expected_color = &manual_color_mapping_function;

/**
 * Waits for @param duration ms. Calls bus.tick() while at it.
 * @return true if state changed to abort_state (the delay was aborted)
 */
template <State abort_state>
bool delay_or_abort_if_state_changed(const int duration)
{
	waitFor([&]() {
		bus.tick();
		return state == abort_state;
	},
			duration);
	return state == abort_state;
}

/**
 * @return true if aborted because the game was finished
 */
bool flash_colors(const SimonColor colors[], const size_t to_inclusive)
{
	for (size_t color_index = 0; color_index <= to_inclusive; ++color_index)
	{
		auto color_to_flash = colors[color_index];
		flash_single_color(color_to_flash);

		if (delay_or_abort_if_state_changed<State::FINISHED>(1000))
			return true;

		turn_all_colors_off();

		if (color_index < to_inclusive)
		{
			if (delay_or_abort_if_state_changed<State::FINISHED>(500))
				return true;
		}
	}
	return false;
}

void turn_all_colors_off()
{
	for (size_t i = 0; i < 4; ++i)
		leds[i].off();
}

void flash_single_color(const SimonColor color)
{
	for (size_t i = 0; i < 4; ++i)
		leds[i].set((byte)color == i);

	sound.on_timed(1000, frequencies[(byte)color]);
}

bool any_button_pressed(void)
{
	for (auto &b : buttons)
	{
		b.tick();
		if (b.is_pressed())
			return true;
	}
	return false;
}

void loop()
{
	static Timestamp last_user_interaction_or_indication = 0;

	bus.tick();
	switch (state)
	{
	case State::WAIT_FOR_SETUP:
		module_status_led.waiting_for_game_setup();
		break;
	case State::WAIT_FOR_START:
		module_status_led.waiting_for_game_start();
		break;
	case State::WAIT_FOR_STRIKE_UPDATE:
		RGB.color(RGBColor::Green | RGBColor::Red);
		break;
	case State::FINISHED:
		RGB.color(RGBColor::Green | RGBColor::Blue);
		break;
	case State::COLOR_FLASHING:
	{
		RGB.color(RGBColor::Blue | RGBColor::Red);

		bool flashing_was_aborted = flash_colors(colors, required);
		if (flashing_was_aborted)
		{
			turn_all_colors_off();
			break;
		}

		last_user_interaction_or_indication = millis();
		state = State::WAIT_FOR_INPUT;
		break;
	}
	case State::WAIT_FOR_INPUT:
	{
		while (true)
		{
			if (millis() - last_user_interaction_or_indication > retry_timeout)
			{
				state = State::COLOR_FLASHING;
				break;
			}

			bool wrong_button_pressed = false;
			bool current_color_pressed = false;

			auto flashed_color = colors[expected_color_index];
			auto expected_color = get_expected_color(global_game_state.has_vowels, global_game_state.strikes, flashed_color);

			while (!wrong_button_pressed && !current_color_pressed)
			{
				bus.tick();

				for (byte color_of_button = 0; color_of_button < 4; ++color_of_button)
				{
					auto &button = buttons[color_of_button];
					button.tick();

					if (button.just_pressed())
					{
						leds[color_of_button].on();
						sound.on(frequencies[color_of_button]);
					}

					if (button.just_released())
					{
						leds[color_of_button].off();
						sound.off();

						last_user_interaction_or_indication = millis();

						if ((SimonColor)color_of_button == expected_color)
							current_color_pressed = true;
						else
							wrong_button_pressed = true;
					}
				}

				if (millis() - last_user_interaction_or_indication > retry_timeout)
				{
					expected_color_index = 0;
					state = State::COLOR_FLASHING;
					return;
				}
			}

			bool any_button_pressed = false;
			do
			{
				bus.tick();

				any_button_pressed = false;
				for (byte color_of_button = 0; color_of_button < 4; ++color_of_button)
				{
					auto &button = buttons[color_of_button];
					button.tick();
					if (button.is_pressed())
					{
						any_button_pressed = true;
						if ((SimonColor)color_of_button != expected_color)
							wrong_button_pressed = true;
					}
				}
				RGB.color(RGBColor::Red | RGBColor::Green);
			} while (any_button_pressed);

			RGB.color(RGBColor::Black);

			if (wrong_button_pressed)
			{
				state = State::STRIKE;
				break;
			}
			else if (current_color_pressed)
			{
				RGB.color(RGBColor::Green);

				if (expected_color_index < required)
				{
					++expected_color_index;
				}
				else
				{
					expected_color_index = 0;
					++required;
					state = required == end
								? State::SOLVED
								: State::COLOR_FLASHING;
					if (state == State::COLOR_FLASHING)
					{
						// Wait a little bit for the next stage, so the user does not get confused
						if (delay_or_abort_if_state_changed<State::FINISHED>(1000))
							break;
					}
					break;
				}
			}
		}
		break;
	}
	case State::STRIKE:
	{
		state = State::WAIT_FOR_STRIKE_UPDATE;
		sound.on_timed(200, 200);
		bus.send(ModuleFailedMessage());
		// Wait a little bit for the new stage, so the user does not get confused
		if (delay_or_abort_if_state_changed<State::FINISHED>(1000))
			break;
		break;
	}
	case State::SOLVED:
		sound.on_timed(400, 300);
		bus.send(ModuleSolvedMessage());
		state = State::FINISHED;
		module_status_led.success();
		break;
	}
}

void setup_handler(const GameMessage &m)
{
	auto &msg = reinterpret_cast<const GameSetupMessage &>(m);
	global_game_state.initialize(msg);

	bus.send(ModuleRegisterMessage(Random::nextInt()));

	state = State::WAIT_FOR_START;
}
void start_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameStartMessage &>(m);
	if (state != State::WAIT_FOR_START)
		return;

	state = State::COLOR_FLASHING;

	module_status_led.off();
	delay(delay_between_start_and_first_flash); // TODO: Make this part of the loop in the state machine
}
void update_handler(const GameMessage &m)
{
	auto &msg = reinterpret_cast<const GameUpdateMessage &>(m);
	global_game_state.update(msg);

	if (state == State::WAIT_FOR_STRIKE_UPDATE)
	{
		// Reset game
		expected_color_index = 0;
		required = 0;
		state = State::COLOR_FLASHING;
	}
}
void lost_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameLostMessage &>(m);
	global_game_state.lost = true;
	state = State::FINISHED;

	module_status_led.fail();
}

/** Returns the same color, regardless of the current game state */
SimonColor identity_color_mapping_function(const bool has_vowel, const Strikes strikes, const SimonColor flashed_color)
{
	return flashed_color;
}
/** Maps a color the same as in the original game manual */
SimonColor manual_color_mapping_function(const bool has_vowel, const Strikes strikes, const SimonColor flashed_color)
{
	if (has_vowel)
	{
		switch (flashed_color)
		{
		case SimonColor::Red:
			switch (strikes)
			{
			case 0:
				return SimonColor::Blue;
			case 1:
				return SimonColor::Yellow;
			case 2:
				return SimonColor::Green;
			}
		case SimonColor::Blue:
			switch (strikes)
			{
			case 0:
				return SimonColor::Red;
			case 1:
				return SimonColor::Green;
			case 2:
				return SimonColor::Red;
			}
		case SimonColor::Green:
			switch (strikes)
			{
			case 0:
				return SimonColor::Yellow;
			case 1:
				return SimonColor::Blue;
			case 2:
				return SimonColor::Yellow;
			}
		case SimonColor::Yellow:
			switch (strikes)
			{
			case 0:
				return SimonColor::Green;
			case 1:
				return SimonColor::Red;
			case 2:
				return SimonColor::Blue;
			}
		}
	}
	else
	{
		switch (flashed_color)
		{
		case SimonColor::Red:
			switch (strikes)
			{
			case 0:
				return SimonColor::Blue;
			case 1:
				return SimonColor::Red;
			case 2:
				return SimonColor::Yellow;
			}
		case SimonColor::Blue:
			switch (strikes)
			{
			case 0:
				return SimonColor::Yellow;
			case 1:
				return SimonColor::Blue;
			case 2:
				return SimonColor::Green;
			}
		case SimonColor::Green:
			switch (strikes)
			{
			case 0:
				return SimonColor::Green;
			case 1:
				return SimonColor::Yellow;
			case 2:
				return SimonColor::Blue;
			}
		case SimonColor::Yellow:
			switch (strikes)
			{
			case 0:
				return SimonColor::Red;
			case 1:
				return SimonColor::Green;
			case 2:
				return SimonColor::Red;
			}
		}
	}
	return SimonColor::Red; // Should not happen
}
