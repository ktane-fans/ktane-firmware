#pragma once

#include <Particle.h>

#include "shared/numbers.hpp"

template <size_t NButtons>
class ButtonArray
{
private:
	static constexpr float adc_resolution = 4095.0f;
	const AnalogPin pin;

public:
	ButtonArray(AnalogPin pin) : pin(pin) { }

	void init()
	{
		pinMode(pin, INPUT);
	}

	bool read_pressed_button(size_t &button_index)
	{
		auto analog_value = analogRead(pin);

		button_index = 0;
		bool found_button = false;

		for (int i = NButtons; i > 0; --i)
		{
				if (analog_value >= (i / ((float)NButtons) * adc_resolution) * 0.75)
			{
				button_index = i - 1;
				found_button = true;
				break;
			}
		}

		return found_button;
	}
};
