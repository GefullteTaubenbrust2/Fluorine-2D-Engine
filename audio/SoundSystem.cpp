#include "SoundSystem.h"
#include "AErrorHandler.h"

namespace fau {
	SFXSystem::SFXSystem(size source_count) : 
	source_count(source_count), sources(new ALuint[source_count]) {

	}

	void SFXSystem::init() {
		audio_al_call(alGenSources(source_count, sources));
	}

	void SFXSystem::setAttenuationAttributes(float min_distace, float rolloff) {
		for (int i = 0; i < source_count; ++i) {
			audio_al_call(alSourcef(sources[i], AL_REFERENCE_DISTANCE, min_distace));
			audio_al_call(alSourcef(sources[i], AL_ROLLOFF_FACTOR, rolloff));
		}
	}

	void SFXSystem::playSound(Buffer& buffer, float volume, float pitch, glm::vec3 position) {
		position -= listener_position;
		for (int i = 0; i < source_count; ++i) {
			ALint state = 0;
			audio_al_call(alGetSourcei(sources[i], AL_SOURCE_STATE, &state));
			if (state != AL_PLAYING) {
				audio_al_call(alSourcei(sources[i], AL_BUFFER, buffer.alBuffer));
				audio_al_call(alSourcef(sources[i], AL_PITCH, pitch));
				audio_al_call(alSourcef(sources[i], AL_GAIN, volume));
				audio_al_call(alSource3f(sources[i], AL_POSITION, position.x, position.y, position.z));
				audio_al_call(alSourcePlay(sources[i]));
				return;
			}
		}
	}

	void SFXSystem::playSound(Buffer& buffer, float volume, float pitch) {
		for (int i = 0; i < source_count; ++i) {
			ALint state = 0;
			audio_al_call(alGetSourcei(sources[i], AL_SOURCE_STATE, &state));
			if (state != AL_PLAYING) {
				audio_al_call(alSourcei(sources[i], AL_BUFFER, buffer.alBuffer));
				audio_al_call(alSourcef(sources[i], AL_PITCH, pitch));
				audio_al_call(alSourcef(sources[i], AL_GAIN, volume));
				audio_al_call(alSource3f(sources[i], AL_POSITION, 0, 0, 0));
				audio_al_call(alSourcePlay(sources[i]));
				return;
			}
		}
	}

	void SFXSystem::dispose() {
		audio_al_call(alDeleteSources(source_count, sources));
		delete[] sources;
	}
}