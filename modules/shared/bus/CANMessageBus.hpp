#pragma once

#include <Particle.h>

#include "MessageBus.hpp"

class CANMessageBus : public MessageBus
{
private:
	const unsigned long baud_rate;
	CANChannel channel;

public:
	CANMessageBus(unsigned long baud_rate = 50000)
		: baud_rate(baud_rate), channel(CANChannel(CAN_D1_D2)) {}

	/** One-Off initialization, moslty done in the setup() method. */
	virtual void init(void) override;

	/** Sends a message to the event bus. */
	virtual void send(const GameMessage &message) override;

	/** Method that will check for new messages (not needed on particle implementations). */
	virtual void tick(void) override;
};
