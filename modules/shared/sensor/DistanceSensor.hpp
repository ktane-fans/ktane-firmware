#pragma once

#include <Particle.h>

#include "../numbers.hpp"

typedef u32 Millimetres;

class DistanceSensor
{
private:
	Pin echo_pin;
	Pin trig_pin;

public:
	DistanceSensor(Pin echo_pin, Pin trig_pin)
		: echo_pin(echo_pin), trig_pin(trig_pin)
	{
	}

	void init();
	Millimetres measure();
};
