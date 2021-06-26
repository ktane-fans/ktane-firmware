#pragma once

#include <Particle.h>

#include "../numbers.hpp"

#define L LOW
#define H HIGH

// TODO: Move this to a cpp file and make this "extern"

const byte segments_array[10][8] = {
	//A B  C  D  E  F  G  H
	{H, H, H, H, H, H, L, L}, // 0
	{L, H, H, L, L, L, L, L}, // 1
	{H, H, L, H, H, L, H, L}, // 2
	{H, H, H, H, L, L, H, L}, // 3
	{L, H, H, L, L, H, H, L}, // 4
	{H, L, H, H, L, H, H, L}, // 5
	{H, L, H, H, H, H, H, L}, // 6
	{H, H, H, L, L, L, L, L}, // 7
	{H, H, H, H, H, H, H, L}, // 8
	{H, H, H, H, L, H, H, L}, // 9
};
