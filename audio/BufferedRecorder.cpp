#include "BufferedRecorder.h"
#include "AErrorHandler.h"
#include "AudioSetup.h"

#define AUDIO_CHUNKSIZE 1024

namespace fau {
	BufferedRecorder::BufferedRecorder(const uint sampleRate, const uint channelCount, Buffer* buffer) : 
		sampleRate(sampleRate), channelCount(channelCount), buffer(buffer) {
		switch (channelCount) {
		case(1):
			format = AL_FORMAT_MONO16;
			break;
		case(2):
			format = AL_FORMAT_STEREO16;
			break;
		default:
			audio_throw_error("only up to 2 channels allowed - setting channel count to 1");
			format = AL_FORMAT_STEREO16;
			BufferedRecorder::channelCount = 1;
			break;
		}
	}

	void BufferedRecorder::operator=(const BufferedRecorder& br) {
		BufferedRecorder::dispose();
		sampleRate = br.sampleRate;
		channelCount = br.channelCount;
		buffer = br.buffer;
		switch (channelCount) {
		case(1):
			format = AL_FORMAT_MONO16;
			break;
		case(2):
			format = AL_FORMAT_STEREO16;
			break;
		default:
			audio_throw_error("only up to 2 channels allowed - setting channel count to 1");
			format = AL_FORMAT_STEREO16;
			BufferedRecorder::channelCount = 1;
			break;
		}
		buffer->sampleRate = sampleRate;
		buffer->channelCount = BufferedRecorder::channelCount;
	}

	void BufferedRecorder::startRecording(const int device_index) {
		if (recording) return;
		buffer->~Buffer();
		buffer->sampleRate = sampleRate;
		buffer->channelCount = BufferedRecorder::channelCount;
		if (device >= 0) {
			audio_al_call(device = alcCaptureOpenDevice(input_devices[device_index].data(), sampleRate, format, sampleRate));
		}
		else {
			audio_al_call(device = alcCaptureOpenDevice(NULL, sampleRate, format, sampleRate));
		}
		recording = true;
		audio_al_call(alcCaptureStart(device));
		data.clear();
		thread = new std::thread(&BufferedRecorder::record, this);
	}

	void BufferedRecorder::record() {
		while (recording) {
			ALCint available;
			audio_al_call(alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 4, &available));
			if (available > 0) {
				const int size = data.size();
				data.resize(size + available * channelCount);
				audio_al_call(alcCaptureSamples(device, &data[size], available));
			}
		}
	}

	void BufferedRecorder::stopRecording() {
		recording = false;
		if (thread) {
			if (thread->joinable()) thread->join();
			delete thread;
			thread = nullptr;
		}
		buffer->samples = new i16[data.size()];
		for (int i = 0; i < data.size(); ++i) {
			buffer->samples[i] = data[i];
		}
		buffer->sampleCount = data.size();
		buffer->init();
		audio_al_call(alcCaptureStop(device));
	}

	void BufferedRecorder::dispose() {
		if (recording) stopRecording();
		data.clear();
		audio_al_call(alcCaptureCloseDevice(device));
	}
}