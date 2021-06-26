#include "utils.hpp"

void flash_led(void) {
	for (int i = 0; i < 2; ++i)
	{
		digitalWrite(D7, HIGH);
		delay(100);
		digitalWrite(D7, LOW);
		delay(100);
	}
}
