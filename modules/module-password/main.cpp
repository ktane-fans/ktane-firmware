#include "main.hpp"

CANMessageBus bus;
ModuleStatusDiode module_status_led(D0, D3);
Button submit_button(A2);
ButtonArray<PASSWORD_LENGTH> upper_buttons(A1);
ButtonArray<PASSWORD_LENGTH> lower_buttons(A0);

State state = State::WAIT_FOR_SETUP;

vector<vector<char>> column_chars = {};
size_t valid_word_index = 0;
size_t current_column_positions[PASSWORD_LENGTH] = {0};

std::unique_ptr<LEDMatrix> led = nullptr;

// Do networking in different thread so the setup method gets called immediately.
SYSTEM_THREAD(ENABLED);

void setup()
{
	RGB.control(true);
	bus.init();
	Random::init();

	module_status_led.init();
	module_status_led.off();

	submit_button.init();

	upper_buttons.init();
	lower_buttons.init();

	led = std::make_unique<LEDMatrix>(5, 1, /* CLK */ A3, /* CS */ A4, /* DIN */ A5);

	led->addMatrix(0, 0, /* rotation */ 0, false, false);
	led->addMatrix(1, 0, /* rotation */ 0, false, false);
	led->addMatrix(2, 0, /* rotation */ 0, false, false);
	led->addMatrix(3, 0, /* rotation */ 0, false, false);
	led->addMatrix(4, 0, /* rotation */ 0, false, false);
	led->setIntensity(15);

	bus.on(GameMessageType::GAME_SETUP, &setup_handler);
	bus.on(GameMessageType::GAME_START, &start_handler);
	bus.on(GameMessageType::GAME_LOST, &lost_handler);

	auto column_configuration = generate_column_configuration();

	valid_word_index = get<0>(column_configuration);
	column_chars = get<1>(column_configuration);

	display_word({'O', 'h', ' ', 'h', 'i'});
	// update_display();
}

void handle_buttons(void)
{
	static bool upper_row_pressed = false;
	static bool lower_row_pressed = false;

	constexpr Duration cheap_debounce_timeout = 30_ms;
	static Timestamp last_update = 0;
	const Timestamp now = millis();

	if (now - last_update > cheap_debounce_timeout)
	{
		last_update = now;

		bool should_update_display = false;

		// We handle button states using a state machine
		// Outer if: Current state
		// Inner if: Transition to take
		size_t pressed_upper_button = 0;
		bool is_upper_button_pressed = upper_buttons.read_pressed_button(pressed_upper_button);
		if (upper_row_pressed)
		{
			if (!is_upper_button_pressed)
			{
				upper_row_pressed = false;
			}
		}
		else
		{
			if (is_upper_button_pressed)
			{
				upper_row_pressed = true;
				increment_column_position(pressed_upper_button);
				should_update_display = true;
			}
		}

		size_t pressed_lower_button = 0;
		bool is_lower_button_pressed = lower_buttons.read_pressed_button(pressed_lower_button);
		if (lower_row_pressed)
		{
			if (!is_lower_button_pressed)
			{
				lower_row_pressed = false;
			}
		}
		else
		{
			if (is_lower_button_pressed)
			{
				lower_row_pressed = true;
				decrement_column_position(pressed_lower_button);
				should_update_display = true;
			}
		}

		if (should_update_display)
		{
			update_display();
		}
	}
}

void loop()
{

	bus.tick();

	switch (state)
	{
	case State::WAIT_FOR_SETUP:
		module_status_led.waiting_for_game_setup();
		break;
	case State::WAIT_FOR_START:
		module_status_led.waiting_for_game_start();
		break;
	case State::RUNNING:
		submit_button.tick();

		if (submit_button.just_released())
		{
			if (column_indexes_are_correct())
			{
				state = State::SOLVED;
			}
			else
			{
				state = State::STRIKE;
			}
			break;
		}

		handle_buttons();

		break;
	case State::SOLVED:
		RGB.color(RGBColor::Blue);
		module_status_led.success();

		bus.send(ModuleSolvedMessage());
		state = State::FINISHED;
		break;
	case State::FINISHED:
		break;
	case State::STRIKE:
		RGB.color(RGBColor::Green);
		state = State::RUNNING;
		bus.send(ModuleFailedMessage());
		break;
	}
}

void setup_handler(const GameMessage &m)
{
	bus.send(ModuleRegisterMessage(Random::nextInt()));
	state = State::WAIT_FOR_START;
}
void start_handler(const GameMessage &m)
{
	state = State::RUNNING;
	module_status_led.off();
	update_display();
}
void lost_handler(const GameMessage &m)
{
	state = State::FINISHED;
	module_status_led.fail();
	display_word({'b', 'o', 'o', 'o', 'm'});
}

void increment_column_position(size_t column_index)
{
	auto current_position_in_column = current_column_positions[column_index];

	auto chars_in_column = column_chars[column_index].size();

	current_column_positions[column_index] = (current_position_in_column + 1) % chars_in_column;
}
void decrement_column_position(size_t column_index)
{
	auto current_position_in_column = current_column_positions[column_index];

	auto chars_in_column = column_chars[column_index].size();

	// Being cautious with size_t underflow here.
	// We're using modular arithmetic to decrement (by incrementing and then doing %)
	current_column_positions[column_index] = (current_position_in_column + (chars_in_column - 1)) % chars_in_column;
}

bool column_indexes_are_correct(void)
{
	auto &solution = available_words[valid_word_index];

	for (size_t i = 0; i < PASSWORD_LENGTH; ++i)
	{
		size_t column_position = current_column_positions[i];
		auto char_of_column = column_chars[i][column_position];

		char expected_char = solution.at(i);

		if (char_of_column != expected_char)
		{
			return false;
		}
	}
	return true;
}

void update_display(void)
{
	led->fillScreen(0);

	constexpr size_t column_offset = 8;

	for (size_t i = 0; i < PASSWORD_LENGTH; ++i)
	{
		size_t column_position = current_column_positions[i];

		auto char_of_column = column_chars[i][column_position];
		led->drawChar((PASSWORD_LENGTH - i - 1) * column_offset, 0, char_of_column, true, false, 1);
	}

	led->flush();
}

void display_word(std::array<char, PASSWORD_LENGTH> word)
{
	led->fillScreen(0);

	constexpr size_t column_offset = 8;

	led->drawChar(4 * column_offset, 0, word[0], true, false, 1);
	led->drawChar(3 * column_offset, 0, word[1], true, false, 1);
	led->drawChar(2 * column_offset, 0, word[2], true, false, 1);
	led->drawChar(1 * column_offset, 0, word[3], true, false, 1);
	led->drawChar(0 * column_offset, 0, word[4], true, false, 1);

	led->flush();
}
