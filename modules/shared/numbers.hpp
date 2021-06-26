#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>

// See: https://www.geeksforgeeks.org/branch-prediction-macros-in-gcc/
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;

typedef u8 byte;

typedef byte Pin;
typedef Pin AnalogPin;
/** On the Photon, P1 and Electron, this function works on pins D0, D1, D2, D3, A4, A5, WKP, RX and TX */
typedef Pin PwmPin;

typedef byte Percentage;

/** Taken from: https://stackoverflow.com/a/9324086 */
template <typename T>
T clamp(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

#include "units/time.hpp"
#include "units/frequency.hpp"
#include "units/resistance.hpp"
