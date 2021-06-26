#pragma once

#include "numbers.hpp"

void base16_encode(const byte in[8], char out[16]);
void base16_decode(const char in[16], byte out[8]);
byte get_value_from_hex_char(char c);
