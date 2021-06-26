#pragma once

#include <Debounce.h>

#include <Particle.h>
#include "numbers.hpp"

class Button
{
private:
	const Pin pin;
	const int interval;
	bool status;
	Debounce debouncer = Debounce();

public:
	Button(Pin pin, int interval = 20) : pin(pin), interval(interval) {}

	void init(void)
	{
		debouncer.attach(pin, INPUT_PULLUP);
		debouncer.interval(interval);
	}

	void tick()
	{
		debouncer.update();
		status = debouncer.read() == LOW;
	}

	bool just_pressed()
	{
		return debouncer.fell();
	}

	bool just_released()
	{
		return debouncer.rose();
	}

	bool is_pressed() const
	{
		return status;
	}
};
