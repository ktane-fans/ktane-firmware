#pragma once

#include <cstdint>

typedef uint32_t Frequency;

constexpr Frequency operator"" _Hz(const unsigned long long int f)
{
	return f;
}

constexpr Frequency operator"" _kHz(const unsigned long long int f)
{
	return f * 1000;
}

constexpr Frequency operator"" _MHz(const unsigned long long int f)
{
	return f * 1000 * 1000;
}

constexpr Frequency operator"" _kHz(const long double f)
{
	return (Frequency)(f * 1000.0);
}

constexpr Frequency operator"" _MHz(const long double f)
{
	return (Frequency)(f * 1000.0 * 1000.0);
}
