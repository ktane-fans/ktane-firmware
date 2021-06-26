#include <vector>

using std::vector;

enum MorseChar
{
	NONE,
	SHORT,
	LONG,
};

const size_t MAX_MORSE_CODE_LENGTH = 5;

const MorseChar digit_characters[10][MAX_MORSE_CODE_LENGTH] = {
	{LONG, LONG, LONG, LONG, NONE},
	{SHORT, LONG, LONG, LONG, LONG},
	{SHORT, SHORT, LONG, LONG, LONG},
	{SHORT, SHORT, SHORT, LONG, LONG},
	{SHORT, SHORT, SHORT, SHORT, LONG},
	{SHORT, SHORT, SHORT, SHORT, SHORT},
	{LONG, SHORT, SHORT, SHORT, SHORT},
	{LONG, LONG, SHORT, SHORT, SHORT},
	{LONG, LONG, LONG, SHORT, SHORT},
	{LONG, LONG, LONG, LONG, SHORT},
};
const MorseChar letter_characters[26][MAX_MORSE_CODE_LENGTH] = {
	{SHORT, LONG, NONE, NONE, NONE},
	{LONG, SHORT, SHORT, SHORT, NONE},
	{LONG, SHORT, LONG, SHORT, NONE},
	{LONG, SHORT, SHORT, NONE, NONE},
	{SHORT, NONE, NONE, NONE, NONE},
	{SHORT, SHORT, LONG, SHORT, NONE},
	{LONG, LONG, SHORT, NONE, NONE},
	{SHORT, SHORT, SHORT, SHORT, NONE},
	{SHORT, SHORT, NONE, NONE, NONE},
	{SHORT, LONG, LONG, LONG, NONE},
	{LONG, SHORT, LONG, NONE, NONE},
	{SHORT, LONG, SHORT, SHORT, NONE},
	{LONG, LONG, NONE, NONE, NONE},
	{LONG, SHORT, NONE, NONE, NONE},
	{LONG, LONG, LONG, NONE, NONE},
	{SHORT, LONG, LONG, SHORT, NONE},
	{LONG, LONG, SHORT, LONG, NONE},
	{SHORT, LONG, SHORT, NONE, NONE},
	{SHORT, SHORT, SHORT, NONE, NONE},
	{LONG, NONE, NONE, NONE, NONE},
	{SHORT, SHORT, LONG, NONE, NONE},
	{SHORT, SHORT, SHORT, LONG, NONE},
	{SHORT, LONG, LONG, NONE, NONE},
	{LONG, SHORT, SHORT, LONG, NONE},
	{LONG, SHORT, LONG, LONG, NONE},
	{LONG, LONG, SHORT, SHORT, NONE},
};

typedef const MorseChar *const EncodedChar;

EncodedChar get_char_encoding(const char c)
{
	char normalized_char = toupper(c);

	if ('0' <= normalized_char && normalized_char <= '9')
	{
		size_t index = normalized_char - '0';
		return digit_characters[index];
	}

	size_t index = normalized_char - 'A';
	return letter_characters[index];
}
