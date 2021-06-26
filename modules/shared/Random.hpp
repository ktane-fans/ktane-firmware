#pragma once

#include <Particle.h>

#include "numbers.hpp"

/** Code taken from https://community.particle.io/t/random-number-generation-between-range/2068/6 */
class Random
{
public:
	static auto get_seed() -> auto
	{
		// For the seed function, see: https://community.particle.io/t/cant-use-srand-time-null-with-particle/25234/2
		return HAL_RNG_GetRandomNumber();
	}

	/** Seeds the random generator. */
	static void init()
	{
		auto seed = get_seed();
		srand(seed);
	}

	static byte next(byte maximumExclusive)
	{
		return rand() % maximumExclusive;
	}

	static byte next(byte minimum, byte maximumExclusive)
	{
		return next(maximumExclusive - minimum) + minimum;
	}

	static uint32_t nextInt()
	{
		return (uint32_t)rand();
	}

	static byte nextByte()
	{
		return (byte)(nextInt() % 256);
	}
};
