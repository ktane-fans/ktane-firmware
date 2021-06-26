#pragma once

#include <string>

#include <Particle.h>

#include "MessageBus.hpp"
#include "../encoding.hpp"


using std::string;

class ParticleMessageBus : public MessageBus
{
private:
	const string topic;
	string device_id;

	void particle_message_received(const char *event, const char *data);

public:
	ParticleMessageBus(const string topic) : MessageBus(), topic(topic) {}

	/** One-Off initialization, moslty done in the setup() method. */
	virtual void init(void) override;
	/** Sends a message to the event bus. */
	virtual void send(const GameMessage &message) override;
	// void tick(void); // Particle Bus does not need a call every loop, so do not override tick()
};
