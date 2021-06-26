#pragma once

#include <Particle.h>
#include <memory>
#include <array>

#include "shared/bus/communication.hpp"
#include "shared/GameState.hpp"
#include "shared/Diode.hpp"
#include "shared/Button.hpp"
#include "shared/Random.hpp"
#include "shared/ModuleStatusDiode.hpp"

#include "password.hpp"
#include "ButtonArray.hpp"

// There are some issues with a "swap" define
#include <ledmatrix-max7219-max7221.h>

void update_display(void);

void setup_handler(const GameMessage &m);
void start_handler(const GameMessage &m);
void lost_handler(const GameMessage &m);

enum class State : byte
{
	// Game init states
	WAIT_FOR_SETUP,
	WAIT_FOR_START,

	RUNNING,

	// General Module states
	SOLVED,
	FINISHED,
	STRIKE,
};

bool column_indexes_are_correct(void);
void update_display(void);
void increment_column_position(size_t column_index);
void decrement_column_position(size_t column_index);

void display_word(std::array<char, 5> word);
