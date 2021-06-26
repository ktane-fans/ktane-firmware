#pragma once

#include <vector>

#include "shared/output/note.hpp"

using std::vector;

vector<Note> melody = {Note::C5, Note::B4, Note::G4, Note::C5, Note::B4, Note::E4, Note::RESET, Note::C5, Note::C4, Note::G4, Note::A4, Note::C5};
vector<int> beats = {16, 16, 16, 8, 8, 16, 32, 16, 16, 16, 8, 8};
