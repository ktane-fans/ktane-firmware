#pragma once

#include <Particle.h>

#include "data.hpp"

class SevenSegmentDigit
{
private:
	const byte pins[8];

	/**
	 * @param segA top
	 * @param segB right-top
	 * @param segC right-bottom
	 * @param segD bottom
	 * @param segE left-bottom
	 * @param segF left-top
	 * @param segG middle
	 * @param segH dot
	 */
	SevenSegmentDigit(const Pin segA, const Pin segB, const Pin segC, const Pin segD, const Pin segE, const Pin segF, const Pin segG, const Pin segH)
		: pins({segA, segB, segC, segD, segE, segF, segG, segH}) {}

public:
	void init();
	void display(const byte number);
	void set(const byte pin_data[8]);
	void clear();
};
