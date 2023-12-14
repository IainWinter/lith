#include "SDLMixerAudioBackend.h"
#include "SDL_mixer.h"
#include "lith/log.h"

void logMixError() {
	lithLog("SDL_mixer error: {}", SDL_GetError());
}

void logUserError(const char* message) {
	lithLog("Audio error: {}", message);
}

void SDLMixerAudioBackend::create() {
	int status = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

	if (status == -1) {
		logMixError();
		throw nullptr;
	}

	status = Mix_AllocateChannels(16);

	if (status == -1) {
		logMixError();
		throw nullptr;
	}
}

void SDLMixerAudioBackend::free() {
	Mix_CloseAudio();
}

void SDLMixerAudioBackend::load(AudioInstance* instance, const AudioDescription& description) {
	if (description.isStream) {
		Mix_Music* music = Mix_LoadMUS(description.sourceName);
		instance->self = (void*)music;
		instance->isStream = true;
	}

	else {
		Mix_Chunk* chunk = Mix_LoadWAV(description.sourceName);
		instance->self = (void*)chunk;
	}
	
	if (!instance->self) {
		logMixError();
		throw nullptr;
	}
}

void SDLMixerAudioBackend::free(AudioInstance* instance) {
	if (instance->isStream) {
		Mix_Music* music = (Mix_Music*)instance->self;
		Mix_FreeMusic(music);
	}

	else {
		Mix_Chunk* chunk = (Mix_Chunk*)instance->self;
		Mix_FreeChunk(chunk);
	}

	*instance = {};
}

void SDLMixerAudioBackend::play(AudioInstance* instance, const AudioDescription& description) {
	int sdlVolume = (int)floor(description.volume * MIX_MAX_VOLUME);

	if (instance->isStream) {
		Mix_Music* music = (Mix_Music*)instance->self;
		int status = Mix_PlayMusic(music, -1);

		if (status == -1) {
			logMixError();
			return;
		}

		Mix_VolumeMusic(sdlVolume);
		instance->channel = lithCHANNEL_MUSIC;
	}

	else {
		Mix_Chunk* chunk = (Mix_Chunk*)instance->self;
		int channel = Mix_PlayChannel(description.channel, chunk, 0);

		if (channel == -1) {
			logMixError();
			return;
		}

		Mix_VolumeChunk(chunk, sdlVolume);
		instance->channel = channel;
	}
}

void SDLMixerAudioBackend::stop(AudioInstance* instance) {
	int status = 0;

	if (instance->isStream) {
		status = Mix_HaltMusic();
	}

	else {
		if (instance->channel == -1) {
			logUserError("Instance was never created");
			return;
		}

		status = Mix_HaltChannel(instance->channel);
	}

	if (status) {
		logMixError();
		throw nullptr;
	}

	instance->channel = lithCHANNEL_NONE;
}

void SDLMixerAudioBackend::pause(AudioInstance* instance) {
	if (instance->isStream) {
		Mix_PauseMusic();
	}

	else {
		if (instance->channel == -1) {
			logUserError("Instance was never created");
			return;
		}

		Mix_Pause(instance->channel);
	}
}

void SDLMixerAudioBackend::resume(AudioInstance* instance) {
	if (instance->isStream) {
		Mix_ResumeMusic();
	}

	else {
		if (instance->channel == -1) {
			logUserError("Instance was never created");
			return;
		}

		Mix_Resume(instance->channel);
	}
}