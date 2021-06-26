#pragma once

#include <cstdint>

typedef int Resistance;

constexpr Resistance operator"" _Ohm(const unsigned long long int f)
{
	return f;
}

constexpr Resistance operator"" _kOhm(const unsigned long long int f)
{
	return f * 1000;
}

constexpr Resistance operator"" _MOhm(const unsigned long long int f)
{
	return f * 1000 * 1000;
}

constexpr Resistance operator"" _kOhm(const long double f)
{
	return (Resistance)(f * 1000.0);
}

constexpr Resistance operator"" _MOhm(const long double f)
{
	return (Resistance)(f * 1000.0 * 1000.0);
}
