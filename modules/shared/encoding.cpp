#include "encoding.hpp"

void base16_encode(const byte in[8], char out[16])
{
	for (size_t i = 0; i < 8; ++i)
	{
		byte current_byte = in[i];

		out[i * 2] = "0123456789ABCDEF"[static_cast<size_t>(current_byte >> 4)];
		out[i * 2 + 1] = "0123456789ABCDEF"[static_cast<size_t>(current_byte & 0x0F)];
	}
}

byte get_value_from_hex_char(char c)
{
	if ('0' <= c && c <= '9') return (c - '0');
	if ('A' <= c && c <= 'F') return (c - 'A' + 10);
	if ('a' <= c && c <= 'f') return (c - 'a' + 10);
	return 0;
}

void base16_decode(const char in[16], byte out[8])
{
	for (size_t i = 0; i < 8; ++i)
	{
		unsigned int nibble_left = get_value_from_hex_char(in[i * 2]);
		unsigned int nibble_right = get_value_from_hex_char(in[i * 2 + 1]);
		byte data = (nibble_left << 4) | nibble_right;
		out[i] = data;
	}
}
