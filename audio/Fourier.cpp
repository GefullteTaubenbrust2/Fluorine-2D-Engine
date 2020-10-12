#include "Fourier.h"
#include <algorithm>

namespace fau {
	double cos_approx(double x)
	{
#if 0
		float const c1 = 0.99940307;
		float const c2 = -0.49558072;
		float const c3 = 0.03679168;
		float const fx = x;
		float const x2 = fx * fx;
		return (double)(c1 + x2*(c2 + c3*x2));
#elif 0
		float const c1 = 0.9999932946;
		float const c2 = -0.4999124376;
		float const c3 = 0.0414877472;
		float const c4 = -0.0012712095;
		float const fx = x;
		float const x2 = fx * fx;
		return (double)(c1 + x2*(c2 + x2*(c3 + c4*x2)));
#elif 0
		double const c1 = 0.999999953464;
		double const c2 = -0.4999999053455;
		double const c3 = 0.0416635846769;
		double const c4 = -0.0013853704264;
		double const c5 = 0.00002315393167;
		double const fx = x;
		double const x2 = fx * fx;
		return (c1 + x2*(c2 + x2*(c3 + x2*(c4 + c5*x2))));
#elif 0
		double const c1 = 0.99999999999925182;
		double const c2 = -0.49999999997024012;
		double const c3 = 0.041666666473384543;
		double const c4 = -0.001388888418000423;
		double const c5 = 0.0000248010406484558;
		double const c6 = -0.0000002752469638432;
		double const c7 = 0.0000000019907856854;
		double const fx = x;
		double const x2 = fx * fx;
		return (c1 + x2*(c2 + x2*(c3 + x2*(c4 + x2*(c5 + x2*(c6 + c7*x2))))));
#elif 1
		double const c1 = 0.99999999999999806767;
		double const c2 = -0.4999999999998996568;
		double const c3 = 0.04166666666581174292;
		double const c4 = -0.001388888886113613522;
		double const c5 = 0.000024801582876042427;
		double const c6 = -0.0000002755693576863181;
		double const c7 = 0.0000000020858327958707;
		double const c8 = -0.000000000011080716368;
		double const fx = x;
		double const x2 = fx * fx;
		return (c1 + x2*(c2 + x2*(c3 + x2*(c4 + x2*(c5 + x2*(c6 + x2*(c7 + c8*x2)))))));
#endif
	}

	double sin_approx(double x)
	{
		x -= PI / 2;
		if (x < 0) x += PI * 2;
		return cos_approx(x);
	}

	FourierBuffer::FourierBuffer(const unsigned int exponent) : 
		exponent(exponent), size(1 << exponent) {
		even = new flo::Complex<double>*[exponent];
		odd = new flo::Complex<double>*[exponent];
		for (int i = 0; i < exponent; ++i) {
			even[i] = new flo::Complex<double>[size >> (i + 1)];
			odd[i] = new flo::Complex<double>[size >> (i + 1)];
		}
		trig = new flo::Complex<double>[size >> 1];
		for (int i = 0; i < size / 2; ++i) {
			const double ang = 2.0 * PI * i / size;
			trig[i] = flo::Complex<double>(std::cos(ang), -std::sin(ang));
		}
	}

	void FourierBuffer::operator=(const FourierBuffer& fb) {
		dispose();
		exponent = fb.exponent;
		size = fb.size;
		even = new flo::Complex<double>*[exponent];
		odd = new flo::Complex<double>*[exponent];
		for (int i = 0; i < exponent; ++i) {
			even[i] = new flo::Complex<double>[size >> (i + 1)];
			odd[i] = new flo::Complex<double>[size >> (i + 1)];
		}
		trig = new flo::Complex<double>[size >> 1];
		for (int i = 0; i < size / 2; ++i) {
			const double ang = 2.0 * PI * i / size;
			trig[i] = flo::Complex<double>(std::cos(ang), -std::sin(ang));
		}
	}

	void FourierBuffer::dispose() {
		if (even && odd) {
			for (int i = 0; i < exponent; ++i) {
				delete[] even[i];
				delete[] odd[i];
			}
		}
		if (even) {
			delete[] even;
			even = nullptr;
		}
		if (odd) {
			delete[] odd;
			odd = nullptr;
		}
		if (trig) {
			delete[] trig;
			trig = nullptr;
		}
	}

	void iterateFFT(flo::Complex<double>* input, FourierBuffer* buffer, const int iteration) {
		const int size = buffer->size >> iteration;
		if (size < 2) return;

		flo::Complex<double>* odd = buffer->odd[iteration];
		flo::Complex<double>* even = buffer->even[iteration];

		for (int i = 0; i < size; ++i) {
			if (i & 1) odd[i >> 1] = input[i];
			else even[i >> 1] = input[i];
		}

		iterateFFT(even, buffer, iteration + 1);
		iterateFFT(odd, buffer, iteration + 1);

		for (int k = 0; k < size / 2; k++) {
			const flo::Complex<double> t = buffer->trig[k << iteration] * odd[k];
			input[k] = even[k] + t;
			input[k + size / 2] = even[k] - t;
		}
	}

	void fft(flo::Complex<double>* input, FourierBuffer* buffer) {
		iterateFFT(input, buffer, 0);
	}

	void ifft(flo::Complex<double>* input, FourierBuffer* buffer) {
		for (int i = 0; i < buffer->size; ++i) input[i] = flo::Complex<double>(input[i].real, -input[i].imag);
		iterateFFT(input, buffer, 0);
		for (int i = 0; i < buffer->size; ++i) input[i] = flo::Complex<double>(input[i].real / buffer->size, -input[i].imag / buffer->size);
	}

	AudioVisualizer::AudioVisualizer(const unsigned int exponent, const float base, const unsigned int peak_frequency) : exponent(exponent), sampleCount(1 << exponent), base(base), peak_frequency(peak_frequency), buf(exponent) {
		std::vector<unsigned int> indexes_raw;
		unsigned int last = 0;
		for (double b = 1; b < std::min(peak_frequency, sampleCount); b *= base) {
			const unsigned int i = (unsigned int)b;
			if (i == last) continue;
			indexes_raw.push_back(i);
			last = i;
		}
		returned_size = indexes_raw.size();
		indexes = new unsigned int[returned_size];
		for (int i = 0; i < returned_size; ++i) indexes[i] = indexes_raw[i];
		output = new double[returned_size];
		input = new flo::Complex<double>[sampleCount];
	}

	double* AudioVisualizer::getSpectrum(i16* samples, const unsigned int sampleRate, const unsigned int sampleCount, const unsigned int channelCount, const double playingOffset) {
		const unsigned int sample = playingOffset * sampleRate * channelCount;
		for (int i = sample; i < sample + AudioVisualizer::sampleCount; ++i) {
			if (i >= sampleCount) input[i - sample] = flo::Complex<double>(0, 0);
			else input[i - sample] = flo::Complex<double>(samples[i], 0);
		}

		fft(input, &buf);

		int last = 0;
		for (int i = 0; i < returned_size; ++i) {
			int m = 0;
			const int index = indexes[i];
			for (int j = last; j < index; ++j) {
				const double mag = input[index].abs();
				if (mag > m) {
					m = mag;
				}
			}
			last = index;
			output[i] = m;
		}
		
		return output;
	}

	void AudioVisualizer::operator=(const AudioVisualizer& av) {
		dispose();
		indexes = new unsigned int[av.returned_size];
		std::copy(av.indexes, av.indexes + av.returned_size, indexes);
		input = new flo::Complex<double>[av.buf.size];
		buf = av.buf;
		output = new double[av.returned_size];
		exponent = av.exponent;
		sampleCount = av.buf.size;
		base = av.base;
		returned_size = av.returned_size;
		peak_frequency = av.peak_frequency;
	}

	void AudioVisualizer::dispose() {
		if (output) {
			delete[] output;
			output = nullptr;
		}
		if (indexes) {
			delete[] indexes;
			indexes = nullptr;
		}
		if (input) {
			delete[] input;
			input = nullptr;
		}
	}

	FilteredBufferStream::FilteredBufferStream(Buffer* buffer, void(*filter)(flo::Complex<double>* samples, const unsigned int array_size, const unsigned int samplePos), const unsigned int exp) :
		buffer(buffer), filter(filter)  {
		FilteredBufferStream::loadCount = 1 << exp;
		FilteredBufferStream::sampleCount = buffer->sampleCount;
		FilteredBufferStream::sampleRate = buffer->sampleRate;
		FilteredBufferStream::channelCount = buffer->channelCount;
		complex_buffer = new flo::Complex<double>[loadCount];
		fbuf = FourierBuffer(exp);
	}

	short* FilteredBufferStream::retrieveSamples(const unsigned int sample) {
		const unsigned int max = std::min(sampleCount, sample + loadCount);
		const unsigned int min = std::max(0u, sample);
		retrieve = std::max((int)max - (int)min, 0);
		eof = min > max;
		for (unsigned int i = min; i < max; ++i) {
			complex_buffer[i - sample] = flo::Complex<double>(buffer->samples[i], 0);
		}
		if (retrieve <= 0) {
			for (int i = 0; i < loadCount; ++i) {
				complex_buffer[i] = flo::Complex<double>(0, 0);
			}
		}
		else if (retrieve < loadCount) {
			if (!min) {
				for (int i = 0; i < -(int)sample; ++i) {
					complex_buffer[i] = flo::Complex<double>(0, 0);
				}
			}
			else if (max == sampleCount) {
				for (int i = loadCount + ((int)sampleCount - (int)sample); i < loadCount; ++i) {
					complex_buffer[i] = flo::Complex<double>(0, 0);
				}
			}
		}
		fft(complex_buffer, &fbuf);
		(*filter)(complex_buffer, loadCount, sample);
		ifft(complex_buffer, &fbuf);
		short* return_buffer = new short[loadCount];
		for (int i = 0; i < loadCount; ++i) {
			return_buffer[i] = complex_buffer[i].real;
		}
		return return_buffer;
	}

	void FilteredBufferStream::operator=(const FilteredBufferStream& fbs) {
		dispose();
		buffer = fbs.buffer;
		filter = fbs.filter;
		loadCount = fbs.loadCount;
		sampleCount = fbs.buffer->sampleCount;
		sampleRate = fbs.buffer->sampleRate;
		channelCount = fbs.buffer->channelCount;
		complex_buffer = new flo::Complex<double>[loadCount];
		fbuf = fbs.fbuf;
	}

	void FilteredBufferStream::dispose() {
		if (complex_buffer) {
			delete[] complex_buffer;
			complex_buffer = nullptr;
		}
	}

	void phase_vocoder(short* in, short* out, const int length_in, const int length_out, const float stretch) {
		const int fft_size = 1 << 12;
		FourierBuffer buffer = FourierBuffer(12);
		flo::Complex<double>* samples = new flo::Complex<double>[fft_size];
		float* phases = new float[fft_size];

		if (stretch > 1.0) {
			int offset_in = 0, offset_out = 0;

			for (int i = 0; i < fft_size; ++i) phases[i] = 0;

			while (offset_in < length_in - fft_size - 1 && offset_out < length_out - fft_size - 1) {
				for (int i = 0; i < fft_size; ++i) {
					samples[i] = flo::Complex<double>(in[i + offset_in], 0.0);
				}
				fft(samples, &buffer);
				for (int i = 0; i < fft_size; ++i) {
					phases[i] -= std::atan2(samples[i].imag, samples[i].real) * i;
					samples[i] = flo::Complex<double>(in[i + offset_in + 1], 0.0);
				}
				fft(samples, &buffer);
				for (int i = 0; i < fft_size; ++i) {
					phases[i] += std::atan2(samples[i].imag, samples[i].real) * i;
					double abs = samples[i].abs();
					//samples[i] = logic::Complex<double>(abs * std::cos(phases[i]), abs * std::sin(phases[i]));
				}
				ifft(samples, &buffer);
				for (int i = 0; i < fft_size; ++i) {
					out[i + offset_out] = samples[i].real;
				}

				offset_out += fft_size;

				if (offset_out >= offset_in * stretch) {
					offset_in += fft_size;
				}
			}
		}

		buffer.dispose();
		delete[] samples;
		delete[] phases;
	}
}