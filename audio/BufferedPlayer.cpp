#include "BufferedPlayer.h"
#include "AErrorHandler.h"

namespace fau {
	BufferedPlayer::BufferedPlayer(Buffer* buffer) : buffer(buffer) {
		sampleRate = buffer->sampleRate;

		audio_al_call(alGenSources((ALuint)1, &source));
		audio_al_call(alSourcef(source, AL_PITCH, 1));
		audio_al_call(alSourcef(source, AL_GAIN, 1));

		source_loaded = true;

		audio_al_call(alSourcei(source, AL_BUFFER, buffer->alBuffer));
	}

	void BufferedPlayer::play() {
		audio_al_call(alSourcePlay(source));
		state = PlayerState::playing;
	}

	void BufferedPlayer::pause() {
		audio_al_call(alSourcePause(source));
		state = PlayerState::paused;
	}

	void BufferedPlayer::stop() {
		audio_al_call(alSourceStop(source));
		state = PlayerState::stopped;
	}

	void BufferedPlayer::setLoop(const bool loop) {
		audio_al_call(alSourcei(source, AL_LOOPING, loop));
		BufferedPlayer::loop = loop;
	}

	bool BufferedPlayer::getLoop() const {
		ALint loop = false;
		audio_al_call(alGetSourcei(source, AL_LOOPING, &loop));
		return loop;
	}

	void BufferedPlayer::setPlayingOffset(const double t) {
		audio_al_call(alSourcei(source, AL_BYTE_OFFSET, t * sampleRate * buffer->channelCount * 2));
		time = t;
	}

	double BufferedPlayer::getPlayingOffset() const {
		ALfloat offset = 0;
		audio_al_call(alGetSourcef(source, AL_SEC_OFFSET, &offset));
		return offset;
	}

	PlayerState BufferedPlayer::getState() {
		ALint oom = 0;
		audio_al_call(alGetSourcei(source, AL_BUFFERS_PROCESSED, &oom));
		if (oom) state = stopped;
		return state;
	}

	void BufferedPlayer::setVolume(const float vol) {
		audio_al_call(alSourcef(source, AL_GAIN, vol));
	}

	float BufferedPlayer::getVolume() const {
		ALfloat vol = 0;
		audio_al_call(alGetSourcef(source, AL_GAIN, &vol));
		return vol;
	}

	void BufferedPlayer::setPitch(const float vol) {
		audio_al_call(alSourcef(source, AL_PITCH, vol));
	}

	float BufferedPlayer::getPitch() const {
		ALfloat pitch = 0;
		audio_al_call(alGetSourcef(source, AL_PITCH, &pitch));
		return pitch;
	}

	void BufferedPlayer::setPosition(const glm::vec3& pos) {
		audio_al_call(alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z));
	}

	glm::vec3 BufferedPlayer::getPosition() const {
		glm::vec3 vec;
		audio_al_call(alGetSource3f(source, AL_POSITION, &vec.x, &vec.y, &vec.z));
		return vec;
	}

	void BufferedPlayer::setVelocity(const glm::vec3& pos) {
		audio_al_call(alSource3f(source, AL_VELOCITY, pos.x, pos.y, pos.z));
	}

	glm::vec3 BufferedPlayer::getVelocity() const {
		glm::vec3 vec;
		audio_al_call(alGetSource3f(source, AL_VELOCITY, &vec.x, &vec.y, &vec.z));
		return vec;
	}

	void BufferedPlayer::dispose() {
		if (source_loaded) { audio_al_call(alDeleteSources(1, &source)); }
		source_loaded = false;
	}

	void BufferedPlayer::operator=(const BufferedPlayer& bp) {
		BufferedPlayer::dispose();
		buffer = bp.buffer;
		sampleRate = buffer->sampleRate;

		audio_al_call(alGenSources((ALuint)1, &source));
		source_loaded = true;
		audio_al_call(alSourcef(source, AL_PITCH, 1));
		audio_al_call(alSourcef(source, AL_GAIN, 1));

		audio_al_call(alSourcei(source, AL_BUFFER, buffer->alBuffer));

		loop = bp.loop;
		setPlayingOffset(bp.getPlayingOffset());
		setVolume(bp.getVolume());
		setPitch(bp.getPitch());
		setPosition(bp.getPosition());
		setVelocity(bp.getVelocity());
		state = PlayerState::stopped;
	}
}