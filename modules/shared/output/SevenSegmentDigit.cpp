#include "SevenSegmentDigit.hpp"

void SevenSegmentDigit::init()
{
	for (auto &pin : pins)
		pinMode(pin, OUTPUT);
}

void SevenSegmentDigit::display(byte number)
{
	if (number < 0 || number > 9)
		return;
	set(segments_array[number]);
}

void SevenSegmentDigit::set(const byte pin_data[8])
{
	for (size_t pin_index = 0; pin_index < 8; ++pin_index)
		digitalWrite(pins[pin_index], pin_data[pin_index]);
}

void SevenSegmentDigit::clear()
{
	for (auto &pin : pins)
		digitalWrite(pin, LOW);
}
