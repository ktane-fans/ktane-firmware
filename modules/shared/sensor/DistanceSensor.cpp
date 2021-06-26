#include "DistanceSensor.hpp"

void DistanceSensor::init()
{
	pinMode(trig_pin, OUTPUT);
	digitalWriteFast(trig_pin, LOW);
	pinMode(echo_pin, INPUT);
	delay(50);
}

Millimetres DistanceSensor::measure()
{
	// Taken from: https://gist.github.com/technobly/349a916fb2cdeb372b5e

	digitalWriteFast(trig_pin, HIGH);
	delayMicroseconds(10);
	digitalWriteFast(trig_pin, LOW);

	u32 duration = pulseIn(echo_pin, HIGH);

	// u32 inches = duration / 74 / 2;
	Millimetres millimetres = duration / 29 / 2 * 10;
	return millimetres;
}
