#pragma once

#include <Particle.h>

#include "numbers.hpp"

/**
 * Based on:
 * https://forum.arduino.cc/index.php?topic=12695.0
 * https://create.arduino.cc/projecthub/SURYATEJA/use-a-buzzer-module-piezo-speaker-using-arduino-uno-89df45
 */
class Buzzer
{
	const Pin pin;

public:
	/**
	 * @param Pin Digital pin
	 */
	Buzzer(const Pin pin)
		: pin(pin) {}

	void init()
	{
		// Intentionally left blank
	}

	void on(Frequency frequency = 2_kHz) const
	{
		on_timed(0, frequency);
	}
	void on_timed(int duration, Frequency frequency = 2_kHz) const
	{
		tone(pin, frequency, duration);
	}
	void off() const
	{
		noTone(pin);
	}

	void beep(const uint32_t delay_between_beeps = 125) const
	{
		on();
		delay(delay_between_beeps);
		off();
		delay(delay_between_beeps);
		on();
		delay(delay_between_beeps);
		off();
		delay(delay_between_beeps);
		on();
		delay(delay_between_beeps);
		off();
	}

	void beepShort(const uint32_t delay_between_beeps = 125) const
	{
		on();
		delay(delay_between_beeps);
		off();
		delay(delay_between_beeps);
		on();
		delay(delay_between_beeps);
		off();
	}
};
