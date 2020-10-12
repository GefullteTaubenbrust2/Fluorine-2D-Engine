#pragma once
#include "FileLoader.h"
#include "../logic/Types.h"

namespace fau {
	/// <summary> 
	/// Stream audio from a recording device
	/// WARNING: The member variables of this struct are read-only
	/// </summary>
	/// \see ThreadedStream
	struct ThreadedRecorder : public ThreadedStream {
	protected:
		std::vector<i16*> data_chunks;
		ALCdevice* device;
		ALenum format;
		std::thread* thread;

		void record();

	public:
		/// <summary> Is the stream recording? (As long as it is recording eof will always be false)</summary>
		/// \see eof
		bool recording = false;

		/// <summary> Construct a ThreadedRecorder</summary>
		/// <param name="sampleRate"> The sample rate</param>
		/// <param name="channelCount"> The channel count</param>
		/// <param name="loadCount"> The amount of samples to stream at once</param>
		ThreadedRecorder(const unsigned int sampleRate, const unsigned int channelCount, const unsigned int loadCount);

		ThreadedRecorder() = default;

		void operator=(const ThreadedRecorder& tr);

		/// <summary> Start recording and clear previously recorded data if present</summary>
		/// <param name="device_index"> The index of the input device in input_devices</param>
		/// \see input_devices
		void startRecording(const int device_index = -1);

		/// <summary> Stop recording, samples will still be available, however</summary>
		void stopRecording();

		/// <summary> Retrieve samples from the stream</summary>
		/// <param name="sample"> The index of the first sample to be loaded</param>
		/// <returns> The retrieved samples as an array with a size equal to the retrieve member</returns>
		/// \see retrieve
		/// \see eof
		virtual i16* retrieveSamples(const unsigned int sample) override;

		/// <summary> The destructor</summary>
		void dispose();
	};
}