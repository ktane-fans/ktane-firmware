#pragma once

#include <cstring>

#include <Particle.h>

#include "Messages.hpp"
#include "../encoding.hpp"
#include "../utils.hpp"

/** Type for callback functions */
typedef void (*MessageHandler)(const GameMessage &message);

union SendableGameMessage {
	byte buffer[MAX_MESSAGE_SIZE];
	GameMessage contents;
};
static_assert(sizeof(SendableGameMessage) == MAX_MESSAGE_SIZE, "sizeof(SendableGameMessage) has to be 8");

/** Interface for BUS implementations */
class MessageBus
{
private:
	MessageHandler handlers[(byte)GameMessageType::NUMBER_OF_MESSAGES] = {nullptr};

	MessageHandler get_handler(const SendableGameMessage &message) const;

protected:
	MessageBus() {}

	void receive_message(const SendableGameMessage &message) const;

	void indicate_activity(void) const;

public:
	/** Register an event handler for the message. Only one handler per type is supported. */
	void on(const GameMessageType message_type, const MessageHandler &callback);

	/** Method to un-register for events of this type */
	void off(const GameMessageType message_type);

	/** One-Off initialization, moslty done in the setup() method. */
	virtual void init(void) = 0;

	/** Sends a message to the event bus. */
	virtual void send(const GameMessage &message) = 0;

	/** Method that will check for new messages (not needed on particle implementations). */
	virtual void tick(void) {}

    virtual ~MessageBus() = default;
};
