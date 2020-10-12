#pragma once
#include "OggLoader.h"

#include "stb_vorbis.c"

#include <iostream>

#include <vector>

namespace fau {
	OggBuffer oggLoadFile(const char* filename) {
		OggBuffer result = OggBuffer();

		int error;
		stb_vorbis* vorbis;
		vorbis = stb_vorbis_open_filename(filename, NULL, NULL);

		stb_vorbis_info info = stb_vorbis_get_info(vorbis);

		if (result.samples) delete[] result.samples;

		result.channelCount = info.channels;
		result.sampleRate = info.sample_rate;
		result.sampleCount = stb_vorbis_stream_length_in_samples(vorbis) * result.channelCount;
		result.samples = new i16[result.sampleCount];

		unsigned int loaded = stb_vorbis_get_samples_short_interleaved(vorbis, result.channelCount, result.samples, result.sampleCount);

		stb_vorbis_close(vorbis);

		return result;
	}

	namespace oggStream {
		std::vector<stb_vorbis*> streams;
		std::vector<int> free;

		int openStream(const char* filename) {
			int lowest_free = 1000000;
			for (int i = 0; i < free.size(); ++i) {
				if (free[i] < lowest_free) lowest_free = free[i];
			}
			if (lowest_free > 100000) {
				streams.resize(streams.size() + 1);
				streams[streams.size() - 1] = stb_vorbis_open_filename(filename, NULL, NULL);
				return streams.size() - 1;
			}
			else {
				for (int i = 0; i < free.size(); ++i) {
					if (free[i] == lowest_free) free.erase(free.begin() + i);
				}
				streams[lowest_free] = stb_vorbis_open_filename(filename, NULL, NULL);
				return lowest_free;
			}
		}

		OggBuffer getInfo(int id) {
			stb_vorbis_info info = stb_vorbis_get_info(streams[id]);
			OggBuffer result = OggBuffer();
			result.samples = nullptr;
			result.channelCount = info.channels;
			result.sampleRate = info.sample_rate;
			result.sampleCount = stb_vorbis_stream_length_in_samples(streams[id]) * result.channelCount;
			return result;
		}

		int loadSamples(i16* samples, unsigned int size, unsigned int offset, unsigned int stream_id, unsigned int channelCount) {
			stb_vorbis_seek(streams[stream_id], offset / channelCount);
			//std::cout << offset << '\n';
			//stb_vorbis_seek_frame(streams[stream_id], 100000);
			return stb_vorbis_get_samples_short_interleaved(streams[stream_id], channelCount, samples, size);
		}

		void closeStream(const int id) {
			stb_vorbis_close(streams[id]);
			free.push_back(id);
		}
	}
}