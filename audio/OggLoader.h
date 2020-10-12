#pragma once
#include "../logic/Types.h"

namespace fau {
	struct OggBuffer {
		i16* samples;
		unsigned int sampleRate, channelCount, sampleCount;

		OggBuffer() = default;
	};
}