#include "lith/audio.h"

static AudioBackendInterface* backend = nullptr;

void registerAudioBackendInterface(AudioBackendInterface* backend) {
	::backend = backend;
}

AudioDescription::AudioDescription()
	: sourceName (nullptr)
	, isStream   (false)
	, channel    (lithCHANNEL_AUTO)
	, volume     (1.0f)
{}

AudioInstance::AudioInstance()
	: self     (nullptr)
	, isStream (false)
	, channel  (lithCHANNEL_NONE)
{}

Audio& Audio::source(const char* sourceName) {
	description.sourceName = sourceName;
	return *this;
}

Audio& Audio::stream() {
	description.isStream = true;
	return *this;
}

Audio& Audio::channel(int channel) {
	description.channel = channel;
	return *this;
}

Audio& Audio::volume(float volume) {
	description.volume = volume;
	return *this;
}

void Audio::load() {
	backend->load(&instance, description);
}

void Audio::free() {
	backend->free(&instance);
}

void Audio::play() {
	backend->play(&instance, description);
}

void Audio::stop() {
	backend->stop(&instance);
}

void Audio::pause() {
	backend->pause(&instance);
}

void Audio::resume() {
	backend->resume(&instance);
}