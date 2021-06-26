#pragma once

#include "../numbers.hpp"

typedef byte Strikes;

const size_t MAX_MESSAGE_SIZE = 8;

// For #pragma pack(1), see: https://stackoverflow.com/a/3318475
enum class GameMessageType : byte
{
	GAME_SETUP,
	GAME_START,
	GAME_UPDATE,
	GAME_LOST,

	MODULE_REGISTER,
	MODULE_SOLVED,
	MODULE_FAILED,

	NUMBER_OF_MESSAGES, // See: https://stackoverflow.com/questions/2102582
	FIRST_MESSAGE = GAME_SETUP,
};

#pragma pack(1)
struct GameMessage
{
	const GameMessageType type;

	GameMessage(GameMessageType type)
		: type(type) {}
};

#pragma pack(1)
struct GameSetupMessage : GameMessage
{
	const Strikes initial_strikes;
	const bool has_vowels;
	const bool last_digit_is_odd;

	GameSetupMessage(byte initial_strikes, bool has_vowels, bool last_digit_is_odd)
		: GameMessage(GameMessageType::GAME_SETUP), initial_strikes(initial_strikes), has_vowels(has_vowels), last_digit_is_odd(last_digit_is_odd) {}
};

#pragma pack(1)
struct GameStartMessage : GameMessage
{
	GameStartMessage()
		: GameMessage(GameMessageType::GAME_START) {}
};

#pragma pack(1)
struct GameUpdateMessage : GameMessage
{
	const Strikes strikes;

	GameUpdateMessage(byte strikes)
		: GameMessage(GameMessageType::GAME_UPDATE), strikes(strikes) {}
};

#pragma pack(1)
struct GameLostMessage : GameMessage
{
	GameLostMessage()
		: GameMessage(GameMessageType::GAME_LOST) {}
};

#pragma pack(1)
struct ModuleRegisterMessage : GameMessage
{
	const uint32_t nonce;

	ModuleRegisterMessage(uint32_t nonce)
		: GameMessage(GameMessageType::MODULE_REGISTER), nonce(nonce) {}
};

#pragma pack(1)
struct ModuleSolvedMessage : GameMessage
{
	// TODO: Refactor to make modules have a unique ID?
	ModuleSolvedMessage()
		: GameMessage(GameMessageType::MODULE_SOLVED) {}
};

#pragma pack(1)
struct ModuleFailedMessage : GameMessage
{
	// TODO: Refactor to make modules have a unique ID?
	ModuleFailedMessage()
		: GameMessage(GameMessageType::MODULE_FAILED) {}
};

/**
 * Assert struct/class sizes because we convert them 1-to-1 from a byte-buffer
 */
static_assert(sizeof(GameMessageType) == 1, "sizeof(GameMessageType) has to be 1");
static_assert(sizeof(GameMessage) == 1, "sizeof(GameMessage) has to be 1");
static_assert(sizeof(GameSetupMessage) <= MAX_MESSAGE_SIZE, "sizeof(GameSetupMessage) has to be max 8");
static_assert(sizeof(ModuleSolvedMessage) <= MAX_MESSAGE_SIZE, "sizeof(ModuleSolvedMessage) has to be max 8");
static_assert(sizeof(ModuleFailedMessage) <= MAX_MESSAGE_SIZE, "sizeof(ModuleFailedMessage) has to be max 8");

class RGBColor
{
public:
	static const uint32_t Red = 0xff0000;
	static const uint32_t Green = 0x00ff00;
	static const uint32_t Blue = 0x0000ff;
	static const uint32_t Black = 0x000000;
	static const uint32_t White = Red | Green | Blue;
};
