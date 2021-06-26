#pragma once

#include <Particle.h>

#include "shared/numbers.hpp"

class PWMDiode
{
private:
	const PwmPin pin;
	const Frequency pwm_frequency;
	bool status = false;

public:
	PWMDiode(const PwmPin pin, const Frequency pwm_frequency = 65535_Hz)
		: pin(pin), pwm_frequency(pwm_frequency) {}

	bool get_status() const
	{
		return status;
	}
	PwmPin get_pin() const
	{
		return pin;
	}

	void init(void)
	{
		pinMode(pin, OUTPUT);
	}

	void set_percentage(Percentage value)
	{
		auto clamped_value = clamp(value, (Percentage)0, (Percentage)100);
		analogWrite(pin, clamped_value, pwm_frequency);
	}

	void on(void)
	{
		if (status)
			return;
		status = true;
		set_percentage(100);
	}

	void off(void)
	{
		if (!status)
			return;
		status = false;
		set_percentage(0);
	}

	void set(bool value)
	{
		if (value)
			on();
		else
			off();
	}
};
