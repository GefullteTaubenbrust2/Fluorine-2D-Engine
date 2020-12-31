#include "ThreadedPlayer.h"

namespace fau {
	ThreadedPlayer::ThreadedPlayer(ThreadedStream* stream) : stream(stream) {
		audio_al_call(alGenSources((ALsizei)1, &source));
	}

	/*void ThreadedPlayer::operator=(ThreadedPlayer& tp) {
		ThreadedPlayer::dispose();
		state = PlayerState::stopped;
		stream = tp.stream;
		loop = tp.loop;
		audio_al_call(alGenSources((ALsizei)1, &source));
		setPlayingOffset(tp.getPlayingOffset());
		req_pause = false;
		req_stop = false;
		buffers_cleared = true;
		setPitch(tp.getPitch());
		setVolume(tp.getVolume());
		setPosition(tp.getPosition());
		setVelocity(tp.getVelocity());
	}*/

	void ThreadedPlayer::run() {
		while (!req_stop && !req_pause) {
			int processed = 0;
			bool outpaced = processed > 1;
			ALint playing = 0;
			audio_al_call(alGetSourcei(source, AL_SOURCE_STATE, &playing));
			audio_al_call(alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed));
			if (playing != AL_PLAYING && !processed) {
				audio_al_call(alSourcePlay(source));
			}
			if (processed > 0) {
				for (; processed > 0; --processed) {
					++bufferid;
					audio_al_call(if (buffer0) { if (q0) alSourceUnqueueBuffers(source, 1, &buffers[0]); q0 = false; } else { if (q1) alSourceUnqueueBuffers(source, 1, &buffers[1]); q1 = false; });
					if (buffer0) {
						audio_al_call(alDeleteBuffers((ALuint)1, &buffers[0]));
						audio_al_call(alGenBuffers((ALuint)1, &buffers[0]));
						short* samples = stream->retrieveSamples((bufferid) * stream->loadCount);
						const int size = stream->retrieve;
						if (stream->eof) {
							if (loop) bufferid = -1;
							else {
								bufferid = -1;
								req_id = 0;
								req_time = 0;
								req_stop = true;
								state = PlayerState::stopped;
								audio_al_call(alSourceStop(source));
								return;
							}
						} else if (size <= 0) {
							--bufferid;
							++processed;
							continue;
						}
						audio_al_call(alBufferData(buffers[0], stream->channelCount == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, samples, size * 2, stream->sampleRate));
						delete[] samples;
						audio_al_call(alSourceQueueBuffers(source, (ALuint)1, buffers));
						q0 = true;
					}
					else {
						audio_al_call(alDeleteBuffers((ALuint)1, &buffers[1]));
						audio_al_call(alGenBuffers((ALuint)1, &buffers[1]));
						short* samples = stream->retrieveSamples((bufferid) * stream->loadCount);
						const uint size = stream->retrieve;
						if (stream->eof) {
							if (loop) bufferid = -1;
							else {
								bufferid = -1;
								req_id = 0;
								req_time = 0;
								req_stop = true;
								state = PlayerState::stopped;
								audio_al_call(alSourceStop(source));
								return;
							}
						} else if (!size) {
							--bufferid;
							++processed;
							continue;
						}
						audio_al_call(alBufferData(buffers[1], stream->channelCount == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, samples, size * 2, stream->sampleRate));
						delete[] samples;
						audio_al_call(alSourceQueueBuffers(source, (ALuint)1, &buffers[1]));
						q1 = true;
					}
					buffer0 = !buffer0;
				}
				if (outpaced) {
					ALint state = 0;
					audio_al_call(alGetSourcei(source, AL_SOURCE_STATE, &state));
					if (state != AL_PLAYING) {
						audio_al_call(alSourcePlay(source));
					}
				}
			}
		}
		if (req_pause) {
			audio_al_call(alSourceStop(source));
			req_id = bufferid - 1;
			audio_al_call(alGetSourcef(source, AL_SEC_OFFSET, &req_time));
		}
	}

	void ThreadedPlayer::play() {
		if (state == PlayerState::playing) {
			stop();
		}
		state = PlayerState::playing;

		bufferid = req_id + 1;
		buffer0 = true;

		req_pause = false;
		req_stop = false;

		if (!buffers_cleared) {
			buffers_cleared = true;
			audio_al_call(alSourceUnqueueBuffers(source, 2, &buffers[0]));
			audio_al_call(alDeleteBuffers((ALsizei)2, &buffers[0]));
		}
		audio_al_call(alGenBuffers((ALsizei)2, &buffers[0]));

		short* samples = stream->retrieveSamples(req_id * stream->loadCount);
		if (!samples) return;
		audio_al_call(alBufferData(buffers[0], (stream->channelCount == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, samples, stream->loadCount * 2, stream->sampleRate));
		delete[] samples;
		samples = stream->retrieveSamples(stream->loadCount * (req_id + 1));
		audio_al_call(alBufferData(buffers[1], stream->channelCount == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, samples, stream->loadCount * 2, stream->sampleRate));
		if (!samples) return;
		delete[] samples;

		audio_al_call(alSourceQueueBuffers(source, (ALuint)2, &buffers[0]));
		audio_al_call(alSourcePlay(source));
		audio_al_call(alSourcei(source, AL_LOOPING, AL_FALSE));
		audio_al_call(alSourcef(source, AL_SEC_OFFSET, req_time));

		buffers_cleared = false;
		req_id = 0;
		req_time = 0;

		thread = new std::thread(&ThreadedPlayer::run, this);
	}

	void ThreadedPlayer::dispose() {
		req_stop = true;
		if (thread) {
			if (thread->joinable()) thread->join();
			delete thread;
			thread = nullptr;
		}
		audio_al_call(alSourceStop(source));
		audio_al_call(alDeleteSources((ALsizei)1, &source));
		audio_al_call(alDeleteBuffers((ALsizei)2, &buffers[0]));
		buffers_cleared = true;
	}

	void ThreadedPlayer::stop() {
		req_stop = true;
		if (thread) {
			if (thread->joinable()) thread->join();
			delete thread;
			thread = nullptr;
		}
		audio_al_call(alSourceStop(source));
		state = PlayerState::stopped;
		req_id = 0;
		req_time = 0;
	}

	void ThreadedPlayer::pause() {
		req_pause = true;
		if (thread) {
			if (thread->joinable()) thread->join();
			delete thread;
			thread = nullptr;
		}
		state = PlayerState::paused;
	}

	PlayerState ThreadedPlayer::getState() {
		return state;
	}

	double ThreadedPlayer::getPlayingOffset() {
		float t = 0;
		audio_al_call(alGetSourcef(source, AL_SEC_OFFSET, &t));
		int bid = bufferid - 1;
		if (bid < 0) bid = 0;
		return t + (float)(bid) * stream->loadCount / (stream->sampleRate * stream->channelCount);
	}

	void ThreadedPlayer::setVolume(const float vol) {
		audio_al_call(alSourcef(source, AL_GAIN, vol));
	}

	float ThreadedPlayer::getVolume() {
		ALfloat vol = 0;
		audio_al_call(alGetSourcef(source, AL_GAIN, &vol));
		return vol;
	}

	void ThreadedPlayer::setPitch(const float vol) {
		audio_al_call(alSourcef(source, AL_PITCH, vol));
	}

	float ThreadedPlayer::getPitch() {
		ALfloat pitch = 0;
		audio_al_call(alGetSourcef(source, AL_PITCH, &pitch));
		return pitch;
	}

	void ThreadedPlayer::setPlayingOffset(const double time) {
		req_stop = true;
		if (thread) {
			if (thread->joinable()) thread->join();
			delete thread;
			thread = nullptr;
		}
		audio_al_call(alSourceStop(source));
		req_time = std::fmod(time, (double)stream->loadCount / (double)(stream->sampleRate * stream->channelCount));
		req_id = std::floor(time * stream->sampleRate * stream->channelCount / stream->loadCount);
		if (state == PlayerState::playing) {
			state = PlayerState::stopped;
			play();
		}
	}

	void ThreadedPlayer::setPosition(const glm::vec3& pos) {
		audio_al_call(alSource3f(source, AL_POSITION, pos.x, pos.y, pos.z));
	}

	glm::vec3 ThreadedPlayer::getPosition() {
		glm::vec3 vec;
		audio_al_call(alGetSource3f(source, AL_POSITION, &vec.x, &vec.y, &vec.z));
		return vec;
	}

	void ThreadedPlayer::setVelocity(const glm::vec3& pos) {
		audio_al_call(alSource3f(source, AL_VELOCITY, pos.x, pos.y, pos.z));
	}

	glm::vec3 ThreadedPlayer::getVelocity() {
		glm::vec3 vec;
		audio_al_call(alGetSource3f(source, AL_VELOCITY, &vec.x, &vec.y, &vec.z));
		return vec;
	}
}