#pragma once
#include <vector>

#include "../logic/MathUtil.h"
#include "FileLoader.h"

#include "../logic/Types.h"

namespace fau {
	/// <summary> A buffer store samples for the fast fourier transform
	/// WARNING: The members of this struct are read-only, unless you know what you are doing
	/// </summary>
	/// \see fft
	/// \see Complex
	struct FourierBuffer {

		/// <summary> exponent = log(size) / log(2)</summary>
		unsigned int exponent;

		/// <summary> The amount of samples in the largest arrays</summary>
		unsigned int size;

		/// <summary> The stored samples</summary>
		flo::Complex<double>** even = nullptr, **odd = nullptr;

		/// <summary> The baked trig
		/// WARNING: read-only!
		/// </summary>
		flo::Complex<double>* trig = nullptr;

		/// <summary> Construct a FourierBuffer
		/// WARNING: This constructor is expensive
		/// </summary>
		/// <param name="exponent"> The size will be 2^exponent</param>
		FourierBuffer(const unsigned int exponent);

		FourierBuffer() = default;

		void operator=(const FourierBuffer& fb);

		/// <summary> The destructor</summary>
		void dispose();
	};

	/// <summary> The fast fourier transform</summary>
	/// <param name="input"> The complex array used for input (the real components originating from your input graph), this array is not unallocated and must have a size of 2^n</param>
	/// <param name="buffer"> A buffer for applying the fft, with matching size</param>
	void fft(flo::Complex<double>* input, FourierBuffer* buffer);

	/// <summary> The inverse fast fourier transform</summary>
	/// <param name="input"> The complex array used for input, this array is not unallocated and must have a size of 2^n</param>
	/// <param name="buffer"> A buffer for applying the fft, with matching size</param>
	void ifft(flo::Complex<double>* input, FourierBuffer* buffer);

	/// <summary> A struct for creating audio visualizers
	/// WARNING: The member variables are read-only
	/// </summary>
	struct AudioVisualizer {
	protected:
		unsigned int* indexes = nullptr;
		flo::Complex<double>* input = nullptr;
		FourierBuffer buf;
		double* output = nullptr;

	public:
		unsigned int exponent;
		unsigned int sampleCount;
		float base;
		unsigned int returned_size;
		unsigned int peak_frequency;

		/// <summary> The constructor
		/// WARNING: The base must be greater than 1
		/// </summary>
		/// <param name="exponent"> The amount of samples to load at once will be 2^exponent</param>
		/// <param name="base"> The higher the base, the less samples will be in the final output, but the more accurate their indexes will be</param>
		/// <param name="peak_frequency"> The lowest frequency that will not be output</param>
		AudioVisualizer(const unsigned int exponent, const float base, const unsigned int peak_frequency);

		AudioVisualizer() = default;

		void operator=(const AudioVisualizer& av);

		/// <summary> Get the fourier transform of samples from one specific point in time, on a (if set up correctly) visually sensible scale</summary>
		/// <param name="samples"> The samples to read from</param>
		/// <param name="sampleRate"> The sampleRate of the source</param>
		/// <param name="sampleCount"> The size of samples</param>
		/// <param name="channelCount"> The amount of channels</param>
		/// <param name="playingOffset"> The first timestamp of the data to be read in seconds</param>
		/// <returns> An internally handled pointer to the spectrum</returns>
		double* getSpectrum(i16* samples, const unsigned int sampleRate, const unsigned int sampleCount, const unsigned int channelCount, const double playingOffset);

		/// <summary> The destructor</summary>
		void dispose();
	};

	/// <summary> A struct for applying filters (like LP, HP, or generally making equalizers) or synthesizing sounds</summary>
	struct FilteredBufferStream : public ThreadedStream {
	protected:
		Buffer* buffer = nullptr;
		FourierBuffer fbuf;
		void(*filter)(flo::Complex<double>* samples, const unsigned int array_size, const unsigned int samplePos) = nullptr;
		flo::Complex<double>* complex_buffer = nullptr;

	public:

		/// <summary> Construct a FilteredBufferStream</summary>
		/// <param name="buffer"> The buffer from which to stream</param>
		/// <param name="filter"> A pointer to the function that will be used for processing the audio (parameters: samples - FFT data from the buffer with a size equal to the loadCount variable | array_size - The size of samples | samplePos - The first sample which is currently being read)</param>
		/// <param name="exp The"> loadCount variable of the stream will be equal to 2^exp</param>
		FilteredBufferStream(Buffer* buffer, void(*filter)(flo::Complex<double>* samples, const unsigned int array_size, const unsigned int samplePos), const unsigned int exp);

		FilteredBufferStream() = default;

		void operator=(const FilteredBufferStream& fbs);

		/// <summary> Get processed data from the stream (the FFT will be executed on the original data, the resulting data is filtered by the filter function, before being converted into a processable form by the IFFT)</summary>
		/// <param name="sample"> The first sample to load from</param>
		/// <returns> The processed data</returns>
		virtual short* retrieveSamples(const unsigned int sample) override;

		void dispose();
	};
}