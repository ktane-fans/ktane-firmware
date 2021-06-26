#include "ParticleMessageBus.hpp"

void ParticleMessageBus::init(void)
{
	device_id = string(System.deviceID().c_str());

	Particle.subscribe(topic.c_str(), &ParticleMessageBus::particle_message_received, this, MY_DEVICES);
}

void ParticleMessageBus::send(const GameMessage &message)
{
	char data[17] = {0};
	base16_encode(reinterpret_cast<const byte *>(&message), data);

	auto particle_target_topic = String(topic.c_str()) + String('-') + String(device_id.c_str());

	Particle.publish(particle_target_topic, data, PRIVATE);
}

void ParticleMessageBus::particle_message_received(const char *event_name, const char *data)
{
	auto event = String(event_name);

	if (!event.startsWith(topic.c_str()))
		return; // Event name seems to be malformed or uninteresting for us

	if (event.endsWith(String(device_id.c_str())))
		return; // Ignore event if it was sent by us.

	auto data_length = strlen(data);
	if (data_length != 16)
		return;

	SendableGameMessage msg{
		.buffer = {0u},
	};

	base16_decode(data, msg.buffer);

	receive_message(msg);
}
