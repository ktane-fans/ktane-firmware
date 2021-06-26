#pragma once

#include <Particle.h>

#include "numbers.hpp"

const int ADC_MIN_VALUE = 0;
const int ADC_MAX_VALUE = 4096 - 1;

class Potentiometer
{
private:
	const AnalogPin pin;
	const int min_value;
	const int max_value;

	const int adc_min_value;
	const int adc_max_value;

	const int num_samples = 5;

public:
	Potentiometer(
		const AnalogPin pin,
		const int min_value,
		const int max_value,
		const int adc_min_value = ADC_MIN_VALUE,
		const int adc_max_value = ADC_MAX_VALUE)
		: pin(pin), min_value(min_value), max_value(max_value), adc_min_value(adc_min_value), adc_max_value(adc_max_value) {}

	void init() const
	{
	}

	int read() const
	{
		int reading = analogRead(pin);

		int value = 0;
		for (int i = 0; i < num_samples; ++i)
		{
			value += map(reading, adc_min_value, adc_max_value, min_value, max_value);
		}
		return value / num_samples;
	}
};
