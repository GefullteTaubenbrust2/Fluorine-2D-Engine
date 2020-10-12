#include "FileWriter.h"

#include <fstream>

namespace fau {
	void assignToBytes(char* bytes, const unsigned short val) {
		bytes[0] = val & 0xff;
		bytes[1] = (val & 0xff00) >> 8;
	}

	void assignToBytes(char* bytes, const unsigned int val) {
		bytes[0] = val & 0xff;
		bytes[1] = (val & 0xff00) >> 8;
		bytes[2] = (val & 0xff0000) >> 16;
		bytes[3] = (val & 0xff000000) >> 24;
	}

	void outputWAVFile(const std::string filename, Buffer* buffer) {
		std::ofstream output(filename, std::ios::binary);

		char* ushort = new char[2];
		char* uint = new char[4];
		output << "RIFF";
		const unsigned int byteCount = buffer->sampleCount * 2;
		assignToBytes(uint, (unsigned int)(byteCount + 36));
		output.write(uint, 4);
		output << "WAVE";
		output << "fmt ";
		assignToBytes(uint, (unsigned int)16);
		output.write(uint, 4);
		assignToBytes(ushort, (unsigned short)1);
		output.write(ushort, 2);
		assignToBytes(ushort, (unsigned short)buffer->channelCount);
		output.write(ushort, 2);
		assignToBytes(uint, (unsigned int)buffer->sampleRate);
		output.write(uint, 4);
		assignToBytes(uint, (unsigned int)(buffer->sampleRate * buffer->channelCount * 2));
		output.write(uint, 4);
		assignToBytes(ushort, (unsigned short)(buffer->channelCount * 2));
		output.write(ushort, 2);
		assignToBytes(ushort, (unsigned short)16);
		output.write(ushort, 2);
		output << "data";
		assignToBytes(uint, (unsigned int)byteCount);
		output.write(uint, 4);
		output.write((char*)buffer->samples, byteCount);

		output.close();
		delete[] uint;
		delete[] ushort;
		std::cout << "finished writing\n";
	}
}