#pragma once

#include <vector>
#include <string>
#include <set>
#include <tuple>
#include <algorithm>

#include "shared/numbers.hpp"
#include "shared/Random.hpp"

using namespace std;

#define PASSWORD_LENGTH 5

extern const vector<char> charset;
extern const vector<string> available_words;

bool is_word_possible(const string &word, const vector<set<char>> &columns);

tuple<size_t, const vector<vector<char>>> generate_column_configuration();
