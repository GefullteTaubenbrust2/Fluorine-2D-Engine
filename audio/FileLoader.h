#pragma once
#include <fstream>

#include "Buffer.h"

#include "../logic/Types.h"

namespace fau {
	/// <summary> Load a .wav file to a buffer
	/// WARNING: The buffer is not unallocated from this function
	/// </summary>
	/// <param name="name"> The name of the file to load </param>
	/// <param name="buffer"> A pointer to the buffer to be loaded to </param>
	/// \see Buffer
	void loadWAVFile(const std::string name, Buffer* buffer);

	/// <summary> Load a .ogg file to a buffer
	/// WARNING: The buffer is not unallocated from this function
	/// </summary>
	/// <param name="name"> The name of the file to load </param>
	/// <param name="buffer"> A pointer to the buffer to be loaded to </param>
	/// \see Buffer
	extern "C" void loadOGGFile(const std::string name, Buffer* buffer);

	/// <summary> A superclass for streaming data (loadCount samples are retrieved at a time)
	/// WARNING: All members of this struct are read-only
	/// </summary>
	struct ThreadedStream {
		unsigned int
			loadCount,
			sampleCount,
			sampleRate,
			channelCount;
		
		/// <summary> The amount of samples retrieved on the last call of retrieveSamples() </summary>
		unsigned int retrieve;

		/// <summary> Is true if on the last call of retrieveSamples() there were samples that could not be loaded and will never be obtainable </summary>
		bool eof = false;

		/// <summary> Retrieve samples from the stream </summary>
		/// <param name="sample"> The index of the first sample to be loaded </param>
		/// <returns> The retrieved samples as an array with a size equal to the retrieve member </returns>
		/// \see retrieve
		/// \see eof
		virtual i16* retrieveSamples(const unsigned int sample) { return nullptr; }
	};

	/// <summary> A struct for streaming from .wav files </summary>
	/// \see ThreadedStream
	struct LoadingStreamWAV : public ThreadedStream {
	protected:
		int bitshift;
		std::ifstream input;

	public:
		LoadingStreamWAV() = default;

		/// <summary> Construct a stream </summary>
		/// <param name="source"> The path and name of the file </param>
		/// <param name="loadCount"> The amount of the samples to be loaded at once </param>
		LoadingStreamWAV(const std::string source, const unsigned int loadCount);

		/// <summary> Retrieve samples from the stream </summary>
		/// <param name="sample"> The index of the first sample to be loaded </param>
		/// <returns> The retrieved samples as an array with a size equal to the retrieve member </returns>
		/// \see retrieve
		/// \see eof
		virtual i16* retrieveSamples(const unsigned int sample) override;

		/// <summary> The destructor </summary>
		~LoadingStreamWAV();
	};

	struct LoadingStreamOGG : public ThreadedStream {
	protected:
		int id;

	public:
		LoadingStreamOGG() = default;

		/// <summary> Construct a stream </summary>
		/// <param name="source"> The path and name of the file </param>
		/// <param name="loadCount"> The amount of the samples to be loaded at once </param>
		LoadingStreamOGG(const std::string source, const unsigned int loadCount);

		/// <summary> Retrieve samples from the stream </summary>
		/// <param name="sample"> The index of the first sample to be loaded </param>
		/// <returns> The retrieved samples as an array with a size equal to the retrieve member </returns>
		/// \see retrieve
		/// \see eof
		virtual i16* retrieveSamples(const unsigned int sample) override;

		/// <summary> The destructor </summary>
		~LoadingStreamOGG();
	};
}