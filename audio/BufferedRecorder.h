#pragma once
#include <al.h>
#include <alc.h>
#include <thread>

#include "Buffer.h"

#include "../logic/Types.h"

namespace fau {
	/// <summary> A struct for recording from a device to a buffer </summary>
	/// \see Buffer
	struct BufferedRecorder {
	protected:
		unsigned int sampleRate;
		unsigned int channelCount;
		std::thread* thread = nullptr;
		std::vector<i16> data;
		Buffer* buffer = nullptr;
		ALCdevice* device;
		ALenum format;

		void record();
	public:
		/// <summary> Is the recorder recording?
		/// WARNING: This variable is read-only
		/// </summary>
		bool recording = false;

		/// <summary> Construct a recorder
		/// WARNING: The buffer is not unallocated from this struct
		/// WARNING: The buffer must not be played when calling this constructor (e.g. BufferedPlayers must have had their destructor called)
		/// WARNING: Whilst recording, the buffer is not usable
		/// </summary>
		/// <param name="sampleRate"> The sampleRate to record at </param>
		/// <param name="channelCount"> The number of channels to record (depending on the device used, unexpected behavior may occur) </param>
		/// <param name="buffer"> The buffer to write to </param>
		BufferedRecorder(const unsigned int sampleRate, const unsigned int channelCount, Buffer* buffer);

		BufferedRecorder() = default;

		void operator=(const BufferedRecorder& br);

		/// <summary> Start the recorder (while recording, changes are not made to the buffer) </summary>
		/// <param name="device_index"> The index of the input device in input_devices </param>
		/// \see input_devices
		void startRecording(const int device_index = -1);

		/// <summary> Stop the recorder and initialize the buffer </summary>
		void stopRecording();

		/// <summary> The destructor </summary>
		void dispose();
	};
}