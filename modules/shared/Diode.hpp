#pragma once

#include <Particle.h>

#include "numbers.hpp"

class Diode
{
private:
	const Pin pin;
	bool status = LOW;

public:
	Diode(const Pin pin) : pin(pin) {}

	bool get_status() const
	{
		return status;
	}
	Pin get_pin() const
	{
		return pin;
	}

	void init(void)
	{
		pinMode(pin, OUTPUT);
	}
	void toggle(void)
	{
		status = !status;
		digitalWrite(pin, status);
	}
	void on(void)
	{
		if (status)
			return;
		status = HIGH;
		digitalWrite(pin, status);
	}
	void off(void)
	{
		if (!status)
			return;
		status = LOW;
		digitalWrite(pin, status);
	}
	void set(bool value)
	{
		status = value ? HIGH : LOW;
		digitalWrite(pin, status);
	}
};
