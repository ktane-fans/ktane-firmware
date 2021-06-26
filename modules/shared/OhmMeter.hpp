#pragma once

#include <Particle.h>

#include "numbers.hpp"

class OhmMeter
{
private:
	const AnalogPin pin;
	const Resistance reference_resistor;

	const float max_val = 4095.0f;
	const float voltage = 3.3f;

public:
	OhmMeter(const AnalogPin pin, const Resistance reference_resistor)
		: pin(pin), reference_resistor(reference_resistor) {}

	float fmap(const float x, const float in_min, const float in_max, const float out_min, const float out_max) const
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	Resistance measure() const
	{
		int val = analogRead(pin);
		float v_out = fmap(val, 0, max_val, 0.0, voltage);
		float unknown_resistor = reference_resistor * v_out / (voltage - v_out);
		return (Resistance)unknown_resistor;
	}
};
