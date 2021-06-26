#pragma once

#include <Particle.h>
#include <LiquidCrystal.h>

#include <string>
#include <vector>
#include <utility>

#include "shared/bus/communication.hpp"
#include "shared/Diode.hpp"
#include "shared/Button.hpp"
#include "shared/Potentiometer.hpp"
#include "shared/Random.hpp"
#include "shared/ModuleStatusDiode.hpp"
#include "shared/utils.hpp"
#include "shared/numbers.hpp"

#include "morse.hpp"
#include "PWMDiode.hpp"

void setup_handler(const GameMessage &m);
void start_handler(const GameMessage &m);
void update_handler(const GameMessage &m);
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
	WAIT_FOR_STRIKE_UPDATE,
};

typedef int Value;

std::vector<std::pair<std::string, Value>> available_words = {
	{"shell", 3505},
	{"halls", 3515},
	{"slick", 3522},
	{"trick", 3532},
	{"boxes", 3535},
	{"leaks", 3542},
	{"strobe", 3545},
	{"bistro", 3552},
	{"flick", 3555},
	{"TODO", 3565},
	{"break", 3572},
	{"brick", 3575},
	{"steak", 3582},
	{"sting", 3592},
	{"vector", 3595},
	{"beats", 3600},
};
