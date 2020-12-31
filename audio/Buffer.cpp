#include "Buffer.h"
#include "AErrorHandler.h"
#include "FileLoader.h"

namespace fau {
	Buffer::Buffer(const std::vector<glm::vec2>& harmonics, const uint sampleRate, const unsigned long long sampleCount, const uint channelCount) :
		sampleRate(sampleRate), sampleCount(sampleCount), channelCount(channelCount) {
		samples = new short[sampleCount];
		const float vol = (1 << 14) / harmonics.size();
		for (int i = 0; i < harmonics.size(); ++i) {
			const float freq = 2.0 * PI * harmonics[i].x / sampleRate;
			for (int j = 0; j < sampleCount; j += channelCount) {
				const int val = std::sin((float)j * freq / channelCount + harmonics[i].y) * vol;
				for (int k = 0; k < channelCount; ++k) {
					if (!i) samples[j + k] = 0;
					samples[j + k] += val;
				}
			}
		}
		init();
	}

	Buffer::Buffer(const std::string& filename, const FileFormat f) {
		samples = new short[1];
		switch (f) {
		case(wav):
			loadWAVFile(filename, this);
			break;
		case(ogg): {
			loadOGGFile(filename, this);
			break;
		}
		default:
			audio_throw_error("unsupported format");
			break;
		}
		init();
	}

	Buffer::Buffer(ThreadedStream* stream) {
		std::thread thread = std::thread(&Buffer::loadFromBuffer, this, stream);
	}

	Buffer::Buffer(i16* samples, const uint sampleCount, const uint sampleRate, const uint channelCount) : 
	sampleCount(sampleCount), channelCount(channelCount), sampleRate(sampleRate) {
		Buffer::samples = new i16[sampleCount];
		std::copy(samples, samples + sampleCount, Buffer::samples);
		init();
	}

	void Buffer::init() {
		if (loaded) return;
		if (!samples) {
			audio_throw_error("The buffer was not yet ready for initialization.");
		}

		audio_al_call(alGenBuffers((ALuint)1, &alBuffer));

		if (channelCount == 2) {
			format = AL_FORMAT_STEREO16;
		}
		else {
			format = AL_FORMAT_MONO16;
		}

		audio_al_call(alBufferData(alBuffer, format, samples, sampleCount * 2, sampleRate));

		loaded = true;
	}

	void Buffer::loadFromBuffer(ThreadedStream* stream) {
		std::vector<short*> collected_data;
		std::vector<uint> lengths;
		uint sample = 0;
		int index = 0;
		while (!stream->eof) {
			short* data = stream->retrieveSamples(sample);
			const uint size = stream->retrieve;
			if (size) {
				collected_data.push_back(new short[stream->loadCount]);
				lengths.push_back(size);
				std::copy(data, data + size, collected_data[index]);
				delete[] data;
				++index;
				sample += size;
			}
		}
		sampleRate = stream->sampleRate;
		channelCount = stream->channelCount;
		for (int i = 0; i < collected_data.size(); ++i) {
			sampleCount += lengths[i];
		}
		samples = new short[sampleCount];
		uint pre = 0;
		for (int i = 0; i < collected_data.size(); ++i) {
			const uint length = lengths[i];
			std::copy(collected_data[i], collected_data[i] + length, samples + pre);
			delete[] collected_data[i];
			pre += length;
		}
		init();
	}

	void Buffer::operator=(const Buffer buf) {
		if (loaded) Buffer::dispose();
		samples = new short[buf.sampleCount];
		std::copy(buf.samples, buf.samples + buf.sampleCount, samples);
		sampleCount = buf.sampleCount;
		channelCount = buf.channelCount;
		sampleRate = buf.sampleRate;
		init();
	}

	void Buffer::dispose() {
		if (samples) delete[] samples;
		samples = nullptr;
		if (loaded) { audio_al_call(alDeleteBuffers((ALsizei)1, &alBuffer)); }
		loaded = false;
	}
}