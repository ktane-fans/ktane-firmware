#include "MessageBus.hpp"

void MessageBus::on(const GameMessageType message_type, const MessageHandler &callback)
{
	handlers[(byte)message_type] = callback;
}
void MessageBus::off(const GameMessageType message_type)
{
	handlers[(byte)message_type] = nullptr;
}

MessageHandler MessageBus::get_handler(const SendableGameMessage &message) const
{
	// first byte of message describes the message type
	auto type = static_cast<GameMessageType>(message.buffer[0]);

	return GameMessageType::FIRST_MESSAGE <= type && type < GameMessageType::NUMBER_OF_MESSAGES
			   ? handlers[(byte)type]
			   : nullptr;
}

void MessageBus::receive_message(const SendableGameMessage &message) const
{
	auto handler = get_handler(message);
	if (handler)
		handler(message.contents);
}

void MessageBus::indicate_activity(void) const
{
	flash_led();
}
