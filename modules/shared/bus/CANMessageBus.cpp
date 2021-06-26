#include "CANMessageBus.hpp"

void CANMessageBus::init(void)
{
	channel.begin(baud_rate);
}

void CANMessageBus::send(const GameMessage &game_message)
{
	CANMessage can_message;
	can_message.id = 0x100u;
	can_message.len = 8;

	can_message.data[0] = 0;
	can_message.data[1] = 0;
	can_message.data[2] = 0;
	can_message.data[3] = 0;
	can_message.data[4] = 0;
	can_message.data[5] = 0;
	can_message.data[6] = 0;
	can_message.data[7] = 0;

	memcpy(&can_message.data, (byte *)&game_message, can_message.len);

	channel.transmit(can_message);
}

void CANMessageBus::tick(void)
{
	CANMessage message;
	if (channel.receive(message))
	{
		// Serial.println(message.id);
		// Serial.println(message.len);

		SendableGameMessage msg{
			.buffer = {0u},
		};

		memcpy(&msg.buffer, &message.data, message.len);

		receive_message(msg);
	}
}
