#pragma once
#include "../logic/Types.h"

namespace fau {
	struct OggBuffer {
		i16* samples = nullptr;
		uint sampleRate = 0, channelCount = 0, sampleCount = 0;

		OggBuffer() = default;
	};
}