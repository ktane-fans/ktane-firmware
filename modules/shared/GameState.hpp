#pragma once

#include "bus/Messages.hpp"

struct GameState
{
	bool lost;
	Strikes strikes;
	bool has_vowels;
	bool last_digit_is_odd;

	void initialize(const GameSetupMessage &message)
	{
		this->lost = false;
		this->has_vowels = message.has_vowels;
		this->strikes = message.initial_strikes;
		this->last_digit_is_odd = message.last_digit_is_odd;
	}

	void update(const GameUpdateMessage &message)
	{
		this->strikes = message.strikes;
	}
};
