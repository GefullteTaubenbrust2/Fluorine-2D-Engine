#include "ThreadedRecorder.h"
#include "AErrorHandler.h"
#include "AudioSetup.h"

namespace fau {
	ThreadedRecorder::ThreadedRecorder(const unsigned int sampleRate, const unsigned int channelCount, const unsigned int loadCount) {
		ThreadedRecorder::sampleRate = sampleRate;
		ThreadedRecorder::channelCount = channelCount;
		ThreadedRecorder::loadCount = loadCount;
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
			ThreadedRecorder::channelCount = 1;
			break;
		}
	}

	void ThreadedRecorder::operator=(const ThreadedRecorder& tr) {
		ThreadedRecorder::dispose();
		loadCount = tr.loadCount;
		sampleCount = tr.sampleCount;
		sampleRate = tr.sampleRate;
		channelCount = tr.channelCount;
		eof = tr.eof;
		retrieve = tr.retrieve;
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
			ThreadedRecorder::channelCount = 1;
			break;
		}
		device = nullptr;
	}

	void ThreadedRecorder::record() {
		while (recording) {
			ALCint availible;
			audio_al_call(alcGetIntegerv(device, ALC_CAPTURE_SAMPLES, 4, &availible));
			const int size = data_chunks.size();
			if (availible >= (loadCount / channelCount)) {
				data_chunks.push_back(new i16[loadCount]);
				audio_al_call(alcCaptureSamples(device, data_chunks[size], loadCount / 2));
				sampleCount = data_chunks.size() * loadCount;
				availible = 0;
			}
		}
	}

	void ThreadedRecorder::startRecording(const int device_index) {
		if (recording) return;
		for (int i = 0; i < data_chunks.size(); ++i) {
			delete[] data_chunks[i];
		}
		recording = true;
		data_chunks.clear();
		if (device >= 0) {
			audio_al_call(device = alcCaptureOpenDevice(input_devices[device_index].data(), sampleRate, format, sampleRate));
		}
		else {
			audio_al_call(device = alcCaptureOpenDevice(NULL, sampleRate, format, sampleRate));
		}
		recording = true;
		audio_al_call(alcCaptureStart(device));
		thread = new std::thread(&ThreadedRecorder::record, this);
	}

	void ThreadedRecorder::stopRecording() {
		recording = false;
		if (thread) {
			if (thread->joinable()) thread->join();
			delete thread;
			thread = nullptr;
		}
		audio_al_call(alcCaptureStop(device));
	}

	i16* ThreadedRecorder::retrieveSamples(const unsigned int sample) {
		i16* output = new i16[loadCount];
		eof = false;
		const unsigned int mod = sample % loadCount;
		if (mod) {
			const int index = sample / loadCount;
			if (index < (int)data_chunks.size() - 1) {
				std::copy(data_chunks[index] + mod, data_chunks[index] + loadCount, output);
				std::copy(data_chunks[index + 1], data_chunks[index + 1] + mod, output + loadCount - mod);
				retrieve = loadCount;
				return output;
			}
			else if (index < data_chunks.size()) {
				if (!recording) eof = true;
				std::copy(data_chunks[index] + mod, data_chunks[index] + loadCount, output);
				for (int i = loadCount - mod; i < loadCount; ++i) output[i] = 0;
				retrieve = loadCount - mod;
				return output;
			}
			else {
				if (!recording) eof = true;
				for (int i = 0; i < loadCount; ++i) {
					output[i] = 0;
				}
				retrieve = 0;
				return output;
			}
		}
		else {
			const int index = sample / loadCount;
			if (index < data_chunks.size()) {
				std::copy(data_chunks[index], data_chunks[index] + loadCount, output);
				retrieve = loadCount;
				return output;
			}
			else {
				if (!recording) eof = true;
				for (int i = 0; i < loadCount; ++i) {
					output[i] = 0;
				}
				retrieve = 0;
				return output;
			}
		}
	}

	void ThreadedRecorder::dispose() {
		if (recording) stopRecording();
		for (int i = 0; i < data_chunks.size(); ++i) {
			delete[] data_chunks[i];
		}
		data_chunks.clear();
	}
}