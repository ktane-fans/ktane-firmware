#include "main.hpp"

CANMessageBus bus;
ModuleStatusDiode module_status_led(D6, D7);

GameState global_game_state{
	.lost = false,
	.strikes = 0,
	.has_vowels = false,
	.last_digit_is_odd = false,
};

constexpr Resistance reference_resistor = 270_Ohm;
const array<WireDetector, MAX_WIRE_COUNT> detectors = {
	WireDetector(A0, reference_resistor),
	WireDetector(A1, reference_resistor),
	WireDetector(A2, reference_resistor),
	WireDetector(A3, reference_resistor),
	WireDetector(A4, reference_resistor),
	WireDetector(A5, reference_resistor),
};

array<WireColor, MAX_WIRE_COUNT> wire_state = {
	WireColor::NONE,
	WireColor::NONE,
	WireColor::NONE,
	WireColor::NONE,
	WireColor::NONE,
	WireColor::NONE,
};

// We use the magic value of 9001 here because -1 would be size_t's max value.
// Checking index_to_cut for < 0 would therefore always be false.
// Instead, we check if it is >= MAX_WIRE_COUNT
// We don't use a signed type here because we'd get a compiler warning that we compare a signed type with an unsigned one.
size_t index_to_cut = 9001;

void update_wire_state()
{
	size_t index = 0;
	for (auto &detector : detectors)
	{
		wire_state[index] = detector.read_wire_color();
		++index;
	}
}

// Do networking in different thread so the setup method gets called immediately.
SYSTEM_THREAD(ENABLED);

void setup()
{
	RGB.control(true);

	module_status_led.init();
	module_status_led.off();

	Random::init();

	update_wire_state();
	index_to_cut = 9001;

	bus.init();

	bus.on(GameMessageType::GAME_SETUP, &setup_handler);
	bus.on(GameMessageType::GAME_START, &start_handler);
	bus.on(GameMessageType::GAME_UPDATE, &update_handler);
	bus.on(GameMessageType::GAME_LOST, &lost_handler);
}

State state = State::WAIT_FOR_SETUP;

State check_for_valid_or_invalid_cut(const State prev_state)
{
	size_t index = 0;
	for (auto &detector : detectors)
	{
		auto new_wire_color = detector.read_wire_color();
		if (wire_state[index] != WireColor::NONE && new_wire_color == WireColor::NONE)
		{
			if (index == index_to_cut)
			{
				return State::SOLVED;
			}
			else
			{
				return State::STRIKE;
			}
		}
		++index;
	}
	return prev_state;
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

		if (index_to_cut >= MAX_WIRE_COUNT)
		{
			index_to_cut = get_index_to_cut(wire_state, global_game_state.last_digit_is_odd);
		}
		break;
	case State::WAIT_FOR_CUT:
	{
		RGB.color(RGBColor::Black);

		state = check_for_valid_or_invalid_cut(state);
		break;
	}
	case State::SOLVED:
		bus.send(ModuleSolvedMessage());
		// needed to check whether the user pulls out another cable after he has won
		update_wire_state();
		state = State::FINISHED;
		module_status_led.success();
		break;
	case State::FINISHED:
		if (!global_game_state.lost)
		{
			// TODO: Remove or handle case that game was lost (using extra state)
			state = check_for_valid_or_invalid_cut(state);
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
	auto &msg = reinterpret_cast<const GameSetupMessage &>(m);
	global_game_state.initialize(msg);

	bus.send(ModuleRegisterMessage(Random::nextInt()));

	state = State::WAIT_FOR_START;
}
void start_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameStartMessage &>(m);
	state = State::WAIT_FOR_CUT;

	module_status_led.off();
}
void update_handler(const GameMessage &m)
{
	auto &msg = reinterpret_cast<const GameUpdateMessage &>(m);
	global_game_state.update(msg);

	if (state == State::WAIT_FOR_STRIKE_UPDATE)
	{
		// Reset game, set state to first actual game state
		update_wire_state();
		state = State::WAIT_FOR_CUT;
	}
}
void lost_handler(const GameMessage &m)
{
	// auto &msg = reinterpret_cast<const GameLostMessage &>(m);
	global_game_state.lost = true;
	state = State::FINISHED;

	module_status_led.fail();
}
