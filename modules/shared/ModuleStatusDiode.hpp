#pragma once

#include "numbers.hpp"
#include "Diode.hpp"

class ModuleStatusDiode
{
private:
	Diode green;
	Diode red;
	unsigned long last_game_status_update = 0;
	const unsigned int STATUS_FLASH_DURATION = 500;

	void flash_status_led(Diode &to_toggle, Diode &to_keep_off);

public:
	ModuleStatusDiode(const Pin green_pin, const Pin red_pin)
		: green(Diode(green_pin)), red(Diode(red_pin)) {}

	void init()
	{
		green.init();
		red.init();
	}

	void waiting_for_game_setup()
	{
		flash_status_led(red, green);
	}

	void waiting_for_game_start()
	{
		flash_status_led(green, red);
	}

	void fail()
	{
		green.off();
		red.on();
	}

	void success()
	{
		green.on();
		red.off();
	}

	void intermediate()
	{
		green.on();
		red.on();
	}

	void off()
	{
		green.off();
		red.off();
	}
};

void ModuleStatusDiode::flash_status_led(Diode &to_toggle, Diode &to_keep_off)
{
	if (last_game_status_update == 0)
		last_game_status_update = millis();

	auto now = millis();
	auto milliseconds_passed = now - last_game_status_update;

	if (milliseconds_passed > STATUS_FLASH_DURATION)
	{
		last_game_status_update = now;
		to_keep_off.off();
		to_toggle.toggle();
	}
}
