#pragma once
#include <vector>
#include <al.h>
#include <alc.h>
#include <thread>

#include "../logic/MathUtil.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../logic/Types.h"

namespace fau {

	/// <summary>  
	/// The formats supported for loading and writing to and from files
	/// </summary>
	enum FileFormat {
		wav = 0,
		ogg = 1,
	};

	struct ThreadedStream;

	/// <summary>  
	/// A struct for storing sounds (a sample only represents a single sample from a single channel)
	/// </summary>
	struct Buffer {
	protected:
		bool loaded = false;

		void loadFromBuffer(ThreadedStream* stream);

	public:
		/// <summary> WARNING: This variable is read-only </summary>
		i16* samples = nullptr;
		/// <summary> WARNING: This variable is read-only </summary>
		uint sampleRate = 0;
		/// <summary> WARNING: This variable is read-only </summary>
		unsigned long long sampleCount = 0;
		/// <summary> WARNING: This variable is read-only </summary>
		uint channelCount = 0;
		/// <summary> WARNING: This variable is read-only </summary>
		uint alBuffer;
		/// <summary> WARNING: This variable is read-only </summary>
		ALenum format;
		
		/// <summary>
		/// Initializes the buffer safely, but is only effective if the buffer was not yet initialized and has its samples loaded
		/// </summary>
		void init();

		Buffer() = default;

		/// <summary>  
		/// Load a buffer from a threaded stream
		/// </summary>
		/// <param name="stream"> A pointer to the stream (you should keep a pointer to it outside of this constructor)</param>
		/// \see ThreadedStream
		Buffer(ThreadedStream* stream);

		/// <summary>  
		/// Create a buffer from harmonics
		/// </summary>
		/// <param name="harmonics"> Each 2D vector represents a sine wave with frequency x and phase offset y</param>
		/// <param name="sampleCount"> The amount of samples</param>
		/// <param name="channelCount"> The amount of channels</param>
		/// <param name="sampleRate"> The rate at which samples are played, divided by channelCount</param>
		Buffer(const std::vector<glm::vec2>& harmonics, const uint sampleRate, const unsigned long long sampleCount, const uint channelCount);

		/// <summary>  
		/// Load a buffer from a file
		/// </summary>
		/// <param name="filename"> The path and name of the file</param>
		/// <param name="f"> The format of the file</param>
		Buffer(const std::string& filename, const FileFormat f);

		/// <summary>  
		/// Create a buffer from scratch
		/// WARNING: An arraysize unequal to sampleCount can lead to undefined behavior without safety checks
		/// </summary>
		/// <param name="samples"> The samples to be copied(!) into the buffer</param>
		/// <param name="sampleCount"> The amount of samples</param>
		/// <param name="channelCount"> The amount of channels</param>
		/// <param name="sampleRate"> The rate at which samples are played, divided by channelCount</param>
		Buffer(i16* samples, const uint sampleCount, const uint sampleRate, const uint channelCount);

		void operator=(const Buffer buf);

		/// <summary> 
		/// The destructor, clears the buffers samples
		/// </summary>
		void dispose();
	};
}