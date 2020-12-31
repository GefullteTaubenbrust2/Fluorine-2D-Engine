#include "AudioSetup.h"
#include "AErrorHandler.h"
#include <iostream>
#include "Buffer.h"

namespace fau {
	ALCdevice* device;
	ALCcontext* context;

	ALint mono_sources;
	ALint stereo_sources;
	std::vector<std::string> output_devices, input_devices;

	void listDevices() {
		const char* raw = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
		const ALCchar* device = raw, * next = raw + 1;
		int len = 0;

		while (device && *device != '\0' && next && *next != '\0') {
			std::string entry = device;
			output_devices.push_back(entry);

			len = strlen(device);
			device += (len + 1);
			next += (len + 2);
		}

		const char* raw2 = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
		device = raw2, next = raw2 + 1;
		len = 0;

		while (device && *device != '\0' && next && *next != '\0') {
			std::string entry = device;
			input_devices.push_back(entry);

			len = strlen(device);
			device += (len + 1);
			next += (len + 2);
		}
	}

	void init(const int device_index) {
		ALCdevice* device;
		if (device_index >= 0) { 
			device = audio_al_call(alcOpenDevice(output_devices[device_index].data())); 
		} 
		else { 
			device = audio_al_call(alcOpenDevice(NULL)); 
		}

		context = audio_al_call(alcCreateContext(device, NULL));
		if (!alcMakeContextCurrent(context)) std::cerr << "An error occured trying to create a context\n";

		float vectors[6] = {
			0., 0., -1.,
			0., 1., 0.
		};

		audio_al_call(alListenerfv(AL_ORIENTATION, vectors));
	}

	void dispose() {
		audio_al_call(alcMakeContextCurrent(NULL));
		audio_al_call(alcDestroyContext(context));
		audio_al_call(alcCloseDevice(device));
	}

	void setMaxSources() {
		audio_al_call(alGetIntegerv(ALC_MONO_SOURCES, &mono_sources));
		audio_al_call(alGetIntegerv(ALC_STEREO_SOURCES, &stereo_sources));
	}

	void allPassFilter(i16* samples, const uint sampleCount, const uint sampleRate) {
		const int delay = 89.27 * sampleRate / 1000;
		for (int i = 0; i < sampleCount; ++i) {
			if (i - delay >= 0) {
				samples[i] += 0.131 * samples[i - delay];
			}
			if (i - delay >= 1) {
				samples[i] += 0.131 * samples[i - delay + 20];
			}
		}

		i16 value = samples[0];
		float max = 0.0f;

		for (int i = 0; i < sampleCount; ++i) {
			float v = std::abs((float)samples[i] / (float)(1 << 15));
			if (v > max) max = v;
		}

		for (int i = 0; i < sampleCount; ++i) {
			short curVal = samples[i];
			value = ((value + curVal - value) / max);
			samples[i] = value;
		}
	}

	void combFilter(i16* samples, const uint sampleCount, const uint sampleRate, const float delay_millis, const float decay_f) {
		const int delay = delay_millis * sampleRate / 1000;
		for (int i = 0; i < sampleCount - delay; ++i) {
			if (i + delay >= 0) samples[i + delay] += (short)(samples[i] * decay_f);
		}
	}

	void reverberate(i16* samples, const uint sampleCount, const uint sampleRate, const uint channelCount, const float delay_millis, const float decay_f) {
		for (int i = 0; i < sampleCount; ++i) {
			samples[i] /= 2;
		}
		combFilter(samples, sampleCount, sampleRate * channelCount, delay_millis, decay_f);
		combFilter(samples, sampleCount, sampleRate * channelCount, delay_millis - 11.73f, decay_f - 0.1313f);
		combFilter(samples, sampleCount, sampleRate * channelCount, delay_millis + 19.31f, decay_f - 0.2743f);
		combFilter(samples, sampleCount, sampleRate * channelCount, delay_millis - 7.97f, decay_f - 0.31f);

		allPassFilter(samples, sampleCount, sampleRate);
		allPassFilter(samples, sampleCount, sampleRate);
	}

	void reverberate(Buffer& buffer, const float delay_millis, const float decay_f) {
		const int delay = delay_millis * buffer.sampleRate / 1000;
		const uint sample_count = buffer.sampleCount * 10;
		i16* data = new i16[sample_count];
		std::copy(buffer.samples, buffer.samples + buffer.sampleCount, data);
		for (int i = buffer.sampleCount; i < sample_count; ++i) data[i] = 0;
		buffer.dispose();
		buffer.samples = data;
		buffer.sampleCount = sample_count;
		reverberate(data, sample_count, buffer.sampleRate, buffer.channelCount, delay_millis, decay_f);
		buffer.init();
	}
}