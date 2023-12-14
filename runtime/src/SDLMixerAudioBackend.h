#pragma once

#include "lith/audio.h"

class SDLMixerAudioBackend : public AudioBackendInterface {
public:
	void create() override;
	void free() override;

	void load(AudioInstance* instance, const AudioDescription& description) override;
	void free(AudioInstance* instance) override;
	void play(AudioInstance* instance, const AudioDescription& description) override;
	void stop(AudioInstance* instance) override;
	void pause(AudioInstance* instance) override;
	void resume(AudioInstance* instance) override;
};