#pragma once

// This is an interface which exposes audio handling. It is up 
// to the runtime to choose the implementation.

// Default is SDL_Audio which is odd in that it plays a single 
// track as 'music' and a limited number of other tracks as 
// effects

#define lithCHANNEL_NONE  -3
#define lithCHANNEL_MUSIC -2
#define lithCHANNEL_AUTO  -1

struct AudioDescription {
	const char* sourceName;
	bool isStream;
	int channel;
	float volume;

	AudioDescription();
};

struct AudioInstance {
	void* self;
	bool isStream;
	int channel;

	AudioInstance();
};

class Audio {
public:
	Audio& source(const char* sourceName);
	
	// If using the SDL_mixer back end, this is needed to 
	// specify a 'music' track, which is streamed from disk.
	// Otherwise will be threated as a 'chunk' and be loaded 
	// into memory in its entirety.
	Audio& stream();

	// With SDL_mixer, the channel max is 8
	// the default is -1 which will auto select one
	Audio& channel(int channel);

	// volume is clamped 0.0 to 1.0
	Audio& volume(float volume);

	void load();
	void free();
	void play();
	void stop();
	void pause();
	void resume();

private:
	AudioDescription description;
	AudioInstance instance;
};

class AudioBackendInterface {
public:
	virtual void create() = 0;
	virtual void free() = 0;

	virtual void load(AudioInstance* instance, const AudioDescription& description) = 0;
	virtual void free(AudioInstance* instance) = 0;
	virtual void play(AudioInstance* instance, const AudioDescription& description) = 0;
	virtual void stop(AudioInstance* instance) = 0;
	virtual void pause(AudioInstance* instance) = 0;
	virtual void resume(AudioInstance* instance) = 0;
};

void registerAudioBackendInterface(AudioBackendInterface* backend);