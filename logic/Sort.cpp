#include "Sort.h"

#include <iostream>

namespace flo {
	void radixSort(i8* values, i8* associated, const int count, const int element_size, char* buffer) {
		if (count < 2) return;

		//first buffer section: values copy
		//second buffer section: indices
		//third buffer section: indices copy
		//fourth buffer section: associated copy
		bool allocate_buffer = buffer == nullptr;
		
		if (allocate_buffer) {
			buffer = new char[count * (element_size + 3)];
		}

		for (int i = 0; i < count; ++i) {
			buffer[i + count] = i;
		}
		std::copy(associated, associated + count * element_size, buffer + count * 3);

		//radix sort for indices using values
		for (int b = 1; b < 256; b <<= 1) {
			int index = 0;
			for (int i = 0; i < count; ++i) {
				const char byte = values[i] & b;
				if (!byte) {
					buffer[index] = values[i];
					buffer[index + count * 2] = buffer[i + count];
					++index;
				}
			}
			for (int i = 0; i < count; ++i) {
				const char byte = values[i] & b;
				if (byte) {
					buffer[index] = values[i];
					buffer[index + count * 2] = buffer[i + count];
					++index;
				}
			}
			b <<= 1;

			//write back to original buffers
			index = 0;
			for (int i = 0; i < count; ++i) {
				const char byte = buffer[i] & b;
				if (!byte) {
					values[index] = buffer[i];
					buffer[index + count] = buffer[i + count * 2];
					++index;
				}
			}
			for (int i = 0; i < count; ++i) {
				const char byte = buffer[i] & b;
				if (byte) {
					values[index] = buffer[i];
					buffer[index + count] = buffer[i + count * 2];
					++index;
				}
			}
		}

		//sort associated values accordingly
		for (int i = 0; i < count; ++i) {
			const int start = count * 3 + element_size * buffer[i + count];
			std::copy(buffer + start, buffer + start + element_size, associated + i * element_size);
		}

		if (allocate_buffer) delete[] buffer;
	}
}