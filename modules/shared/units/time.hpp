#pragma once

#include <cstdint>

typedef uint32_t Duration;

constexpr Duration operator"" _ms(const unsigned long long int ms)
{
	return ms;
}

constexpr Duration operator"" _s(const unsigned long long int s)
{
	return 1000_ms * s;
}

constexpr Duration operator"" _min(const unsigned long long int m)
{
	return 60_s * m;
}

constexpr Duration operator"" _h(const unsigned long long int h)
{
	return 60_min * h;
}

typedef uint32_t Timestamp;
