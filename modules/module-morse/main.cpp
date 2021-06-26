#include "main.hpp"

CANMessageBus bus;

Potentiometer poti(A0, 3500, 3600);
PWMDiode morse_diode(RX);
Button submit_button(A2);
ModuleStatusDiode module_status_led(A5, A4);
LiquidCrystal display(D0, D3, D4, D5, D6, D7);

// Timings taken from: @Syber-space
// Ref: https://www.reddit.com/r/ktane/comments/o6h52g/a_harmless_working_ktane_replica_with_exchangable/h2smpmm/
constexpr Duration SHORT_DURATION = 283_ms;
constexpr Duration LONG_DURATION = 816_ms;
constexpr Duration DURATION_BETWEEN_FLASHES = 1083_ms;
constexpr Duration DURATION_BETWEEN_CHARS = 2700_ms;
constexpr Duration DURATION_BETWEEN_TRANSMISSIONS = 4_s;

State state = State::WAIT_FOR_SETUP;

size_t random_word_index;
Value expected_value;

SYSTEM_THREAD(ENABLED);
Thread *morse_thread = nullptr;

/**
 * Waits for @param duration ms.
 * @return true if state changed to State::FINISHED (the delay was aborted)
 */
bool delay_while_not_finished(const int duration)
{
	waitFor([]() { return state == State::FINISHED; }, duration);
	return state == State::FINISHED;
}

void flash_morse_char(const MorseChar morse_char)
{
	morse_diode.on();
	switch (morse_char)
	{
	case MorseChar::SHORT:
		delay_while_not_finished(SHORT_DURATION);
		break;
	case MorseChar::LONG:
		delay_while_not_finished(LONG_DURATION);
		break;
	}
	morse_diode.off();
}

void display_word(const std::string &word)
{
	size_t char_index = 0;
	for (auto c : word)
	{
		auto encoded_char = get_char_encoding(c);
		for (size_t i = 0; i < MAX_MORSE_CODE_LENGTH; ++i)
		{
			auto morse_char = encoded_char[i];
			if (morse_char == NONE)
				break;

			flash_morse_char(morse_char);

			if (i < MAX_MORSE_CODE_LENGTH - 1 && encoded_char[i + 1] != NONE)
			{
				if (delay_while_not_finished(DURATION_BETWEEN_FLASHES))
					return;
			}
		}

		if (char_index < word.size() - 1)
		{
			if (delay_while_not_finished(DURATION_BETWEEN_CHARS))
				return;
		}

		++char_index;
	}
}

void setup()
{
	RGB.control(true);

	bus.init();
	poti.init();
	submit_button.init();

	module_status_led.init();
	module_status_led.off();

	Random::init();

	// random_word_index = 0;
	random_word_index = Random::next((byte)available_words.size());
	expected_value = available_words[random_word_index].second;

	morse_diode.init();

	display.begin(8, 2);
	display.setCursor(0, 1);
	display.print("MHz");

	bus.on(GameMessageType::GAME_SETUP, &setup_handler);
	bus.on(GameMessageType::GAME_START, &start_handler);
	bus.on(GameMessageType::GAME_UPDATE, &update_handler);
	bus.on(GameMessageType::GAME_LOST, &lost_handler);
}

void display_message(void *param)
{
	auto entry = available_words[random_word_index];
	auto word = entry.first;

	while (state != State::FINISHED)
	{
		display_word(word);
		if (delay_while_not_finished(DURATION_BETWEEN_TRANSMISSIONS))
			return;
	}
}

Value round_value_to_nearest_meaningful_number(const Value v)
{
	auto last_digit = v % 10;
	switch (last_digit)
	{
	case 0:
		return v;
	case 1:
		return v - 1;
	case 2:
		return v;
	case 3:
		return v - 1;
	case 4:
		return v + 1;
	case 5:
		return v;
	case 6:
		return v + 1;
	case 7:
		return v;
	case 8:
		return v - 1;
	case 9:
		return v + 1;
	default:
		return v;
	}
}

void loop()
{
	bus.tick();
	submit_button.tick();

	switch (state)
	{
	case State::WAIT_FOR_SETUP:
	{
		module_status_led.waiting_for_game_setup();
		RGB.color(RGBColor::White); // TODO: Remove
		auto poti_value = round_value_to_nearest_meaningful_number(poti.read());
		display.setCursor(3, 0);
		display.print(poti_value);
		break;
	}
	case State::WAIT_FOR_START:
	{
		module_status_led.waiting_for_game_start();
		RGB.color(RGBColor::Blue); // TODO: Remove
		auto poti_value = round_value_to_nearest_meaningful_number(poti.read());
		display.setCursor(3, 0);
		display.print(poti_value);
		break;
	}
	case State::RUNNING:
	{
		RGB.color(RGBColor::Black);

		auto poti_value = round_value_to_nearest_meaningful_number(poti.read());
		display.setCursor(3, 0);
		display.print(poti_value);

		if (submit_button.just_released())
		{
			if (poti_value == expected_value)
			{
				state = State::SOLVED;
			}
			else
			{
				state = State::STRIKE;
			}
		}
		break;
	}
	case State::SOLVED:
		RGB.color(RGBColor::Blue);
		module_status_led.success();

		bus.send(ModuleSolvedMessage());
		state = State::FINISHED;
		break;
	case State::FINISHED:
		if (morse_thread)
		{
			morse_thread->dispose();
			morse_thread = nullptr;
		}
		break;

	case State::STRIKE:
		RGB.color(RGBColor::Green);
		state = State::WAIT_FOR_STRIKE_UPDATE;
		bus.send(ModuleFailedMessage());
		break;
	case State::WAIT_FOR_STRIKE_UPDATE:
		RGB.color(RGBColor::Green | RGBColor::Red);
		break;
	}
}

void setup_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameSetupMessage &>(m);

	bus.send(ModuleRegisterMessage(Random::nextInt()));

	state = State::WAIT_FOR_START;
}
void start_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameStartMessage &>(m);

	morse_thread = new Thread("morse_thread", &display_message);

	state = State::RUNNING;

	module_status_led.off();
}
void update_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameUpdateMessage &>(m);

	if (state == State::WAIT_FOR_STRIKE_UPDATE)
	{
		// TODO Reset game, set state to first actual game state
		state = State::RUNNING;
	}
}
void lost_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameLostMessage &>(m);
	state = State::FINISHED;

	module_status_led.fail();
}
