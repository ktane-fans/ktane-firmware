#pragma once

#include <Particle.h>
#include <array>

#include "data.hpp"

/**
 * B..H of the shift register are connected to GFABEDC.
 * **We don't use the output A of the shift registers**.
 *
 *     +--A--+
 *     |     |
 *     F     B
 *     |     |
 *     +--G--+
 *     |     |
 *     E     C
 *     |     |
 *     +--D--+  .DP/H
 *
 * This enum just defined which pin of the seven segment display is conected to which pin of the shift register.
 */
enum SevenSegmentChar : byte
{
	//           ABCDEFGH <- Shift register pins
	//           .GFABEDC <- corresponding 7-Segment pins
	SEVSEG_A = 0b00010000,
	SEVSEG_B = 0b00001000,
	SEVSEG_C = 0b00000001,
	SEVSEG_D = 0b00000010,
	SEVSEG_E = 0b00000100,
	SEVSEG_F = 0b00100000,
	SEVSEG_G = 0b01000000,
	SEVSEG_DOT = 0b10000000, // unused in this case
};

/**
 * This is the actual data that will be shifted for each character.
 * As pins might change, we just binary-or the pins needed for a respective segment.
 */
const byte shift_segments_array[10] = {
	/* 0 */ SEVSEG_A | SEVSEG_B | SEVSEG_C | SEVSEG_D | SEVSEG_E | SEVSEG_F,
	/* 1 */ SEVSEG_B | SEVSEG_C,
	/* 2 */ SEVSEG_A | SEVSEG_B | SEVSEG_G | SEVSEG_E | SEVSEG_D,
	/* 3 */ SEVSEG_A | SEVSEG_B | SEVSEG_G | SEVSEG_C | SEVSEG_D,
	/* 4 */ SEVSEG_F | SEVSEG_G | SEVSEG_B | SEVSEG_C,
	/* 5 */ SEVSEG_A | SEVSEG_F | SEVSEG_G | SEVSEG_C | SEVSEG_D,
	/* 6 */ SEVSEG_A | SEVSEG_F | SEVSEG_G | SEVSEG_E | SEVSEG_C | SEVSEG_D,
	/* 7 */ SEVSEG_A | SEVSEG_B | SEVSEG_C,
	/* 8 */ SEVSEG_A | SEVSEG_B | SEVSEG_C | SEVSEG_D | SEVSEG_E | SEVSEG_F | SEVSEG_G,
	/* 9 */ SEVSEG_A | SEVSEG_B | SEVSEG_C | SEVSEG_D | SEVSEG_F | SEVSEG_G,
};

enum class SegmentChar
{
	NONE = 0,
	A = SEVSEG_A | SEVSEG_B | SEVSEG_C | SEVSEG_E | SEVSEG_F | SEVSEG_G,
	B = SEVSEG_E | SEVSEG_G | SEVSEG_C | SEVSEG_D | SEVSEG_F,
	H = SEVSEG_B | SEVSEG_C | SEVSEG_E | SEVSEG_F | SEVSEG_G,
	O = SEVSEG_E | SEVSEG_G | SEVSEG_C | SEVSEG_D,
	I = SEVSEG_F | SEVSEG_E,
	N = SEVSEG_E | SEVSEG_G | SEVSEG_C,
};

template <size_t NDigits>
class SevenSegmentArray
{
private:
	/**
	 * aka SI/"Serial In"
	 */
	const Pin dataPin;
	/**
	 * aka "SCK"
	 */
	const Pin clockPin;
	/**
	 * aka "RCK"
	 */
	const Pin latchPin;

	void display_digit(const byte digit)
	{
		const auto data = shift_segments_array[digit];

		shiftOut(dataPin, clockPin, LSBFIRST, data);
	}

public:
	/**
	 * @param dataPin aka SI/"Serial In"
	 * @param clockPin aka "SCK"
	 * @param latchPin aka "RCK"
	 */
	SevenSegmentArray(const Pin dataPin, const Pin clockPin, const Pin latchPin)
		: dataPin(dataPin), clockPin(clockPin), latchPin(latchPin) {}

	void init()
	{
		pinMode(dataPin, OUTPUT);
		pinMode(clockPin, OUTPUT);
		pinMode(latchPin, OUTPUT);
	}

	/**
	 * This function takes an array of length NDigits, where NDigits is a class-wide constant.
	 *
	 * We do a compile-time check for the correct number of digits passed to this function:
	 * SevenSegmentArray<5> output;
	 * output.display({1, 2, 3, 4, 5}); // ok
	 * output.display({1, 2, 3, 4, 5, 6}); // errors
	 *
	 * We could use a different approach using std::enable_if:
	 * https://stackoverflow.com/a/9101316
	 * But this does not allow us to constrain the type of the parameter pack to be bytes.
	 *
	 * Depending on how the compiler optimizes this function call (using an array), it would be much better to expand the function arguments instead.
	 */
	void display(const std::array<byte, NDigits> digits)
	{
		digitalWrite(latchPin, LOW);

		for (auto &digit : digits)
			display_digit(digit);

		digitalWrite(latchPin, HIGH);
	}

	/** Displays a single number */
	void display(uint32_t number)
	{
		digitalWrite(latchPin, LOW);
		for (size_t i = 0; i < NDigits; ++i)
		{
			const byte digit = number % 10;
			number /= 10;
			display_digit(digit);
		}
		digitalWrite(latchPin, HIGH);
	}

	/** Displays a single number */
	void displaySegments(const std::array<SegmentChar, NDigits> segmentChars)
	{
		digitalWrite(latchPin, LOW);
		for (size_t i = 0; i < NDigits; ++i)
		{
			shiftOut(dataPin, clockPin, LSBFIRST, (byte)segmentChars[i]);
		}
		digitalWrite(latchPin, HIGH);
	}

	void clear()
	{
		digitalWrite(latchPin, LOW);
		for (size_t i = 0; i < NDigits; ++i)
		{
			shiftOut(dataPin, clockPin, LSBFIRST, 0);
		}
		digitalWrite(latchPin, HIGH);
	}

	void full()
	{
		digitalWrite(latchPin, LOW);
		for (size_t i = 0; i < NDigits; ++i)
		{
			shiftOut(dataPin, clockPin, LSBFIRST, 0b11111111);
		}
		digitalWrite(latchPin, HIGH);
	}
};
