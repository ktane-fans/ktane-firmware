#include "password.hpp"

const byte MIN_CHARS_PER_COLUMN = 4;
const byte MAX_CHARS_PER_COLUMN = 6;

// Use lower-case vector for easier debugging
// const vector<char> charset = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
const vector<char> charset = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const vector<string> available_words = {
	"ABOUT",
	"AFTER",
	"AGAIN",
	"BELOW",
	"COULD",
	"EVERY",
	"FIRST",
	"FOUND",
	"GREAT",
	"HOUSE",
	"LARGE",
	"LEARN",
	"NEVER",
	"OTHER",
	"PLACE",
	"PLANT",
	"POINT",
	"RIGHT",
	"SMALL",
	"SOUND",
	"SPELL",
	"STILL",
	"STUDY",
	"THEIR",
	"THERE",
	"THESE",
	"THING",
	"THINK",
	"THREE",
	"WATER",
	"WHERE",
	"WHICH",
	"WORLD",
	"WOULD",
	"WRITE",
};

bool is_word_possible(const string &word, const vector<set<char>> &columns)
{
	for (size_t column_index = 0; column_index < columns.size(); ++column_index)
	{
		auto &chars_of_column = columns[column_index];
		char char_of_word = word[column_index];
		if (chars_of_column.count(char_of_word) == 0)
			return false;
	}
	return true;
}

tuple<size_t, const vector<vector<char>>> generate_column_configuration()
{
	size_t random_word_index = 0;
	byte possible_valid_words = 0;

	vector<set<char>> column_chars = {
		set<char>(),
		set<char>(),
		set<char>(),
		set<char>(),
		set<char>(),
	};

	do
	{
		for (auto &s : column_chars)
			s.clear();

		random_word_index = Random::next((byte)available_words.size());

		auto &word = available_words[random_word_index];

		for (size_t column_index = 0; column_index < column_chars.size(); ++column_index)
		{
			column_chars[column_index].insert(word[column_index]);
		}

		for (size_t column_index = 0; column_index < column_chars.size(); ++column_index)
		{
			auto &current_column = column_chars[column_index];

			byte chars_per_column = Random::next(MIN_CHARS_PER_COLUMN, MAX_CHARS_PER_COLUMN + 1);

			--chars_per_column; // there is already one char in the column

			for (byte i = 0; i < chars_per_column; ++i)
			{
				char c;
				do
				{
					c = charset[Random::next((byte)charset.size())];
				} while (current_column.count(c) != 0); // ugly way of writing ".contains": https://stackoverflow.com/a/1701855
				// C++20 has .contains: https://en.cppreference.com/w/cpp/container/set/contains

				current_column.insert(c);
			}
		}

		possible_valid_words = 0;
		for (auto &word : available_words)
		{
			if (is_word_possible(word, column_chars))
				++possible_valid_words;
		}

	} while (possible_valid_words != 1);

	vector<vector<char>> result;
	for (auto &column : column_chars)
	{
		// Copy set to vector: https://stackoverflow.com/a/5034315
		vector<char> chars(column.begin(), column.end());
		chars.shrink_to_fit();

		result.push_back(chars);
	}
	result.shrink_to_fit();

	// If we dont shuffle the result, the first char would always be the right one
	// To use random_shuffle, we need a data structure that has direct access by index (which a set does not have)
	// So we use a vector.
	// Also, vector will be more practical later when dealing with printing the stuff to the display (we will also need random access)
	// minstd_rand0 random_engine(Random::get_seed());
	for (auto &column : result)
	{
		// TODO: This function may use a random generator with a static seed,
		// since the C++ libs don't know about the photon's hardware random source
		random_shuffle(column.begin(), column.end());
	}

	return make_tuple(random_word_index, result);
}
