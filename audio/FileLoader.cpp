#include <cstring>
#include <stdio.h>
//#include <vorbis/codec.h>
//#include <vorbis/vorbisfile.h>

#include "OggLoader.h"

#include "FileLoader.h"
#include "AErrorHandler.h"

namespace fau {
	uint bytesToEndian(const char bytes[4], const bool big) {
		if (big) {
			return ((uint)(unsigned char)bytes[0] << 24) | ((uint)(unsigned char)bytes[0] << 16) | ((uint)(unsigned char)bytes[2] << 8) | ((uint)(unsigned char)bytes[3]);
		}
		else {
			return ((uint)(unsigned char)bytes[3] << 24) | ((uint)(unsigned char)bytes[2] << 16) | ((uint)(unsigned char)bytes[1] << 8) | ((uint)(unsigned char)bytes[0]);
		}
	}
		
	fau::OggBuffer oggLoadFile(const char* filename);

	namespace oggStream {
		int openStream(const char* filename);
		int loadSamples(short* samples, uint size, uint offset, uint stream_id, uint channelCount);
		void closeStream(const int id);
		OggBuffer getInfo(int id);
	}
}

void fau::loadWAVFile(const std::string name, fau::Buffer* buffer) {
	buffer->~Buffer();
	std::ifstream input(name, std::ios::binary);
	if (!input.is_open()) { audio_throw_error("failed to open file"); return; }
	char chars[4];
	input.read(chars, 4); //chunkid
	if (strncmp(chars, "RIFF", 4) != 0)
	{
		audio_throw_error("this is not a valid WAVE file");
		return;
	}
	input.read(chars, 4);//chunksize
	input.read(chars, 4);//format
	if (strncmp(chars, "WAVE", 4) != 0)
	{
		audio_throw_error("this is not a valid WAVE file");
		return;
	}
	input.read(chars, 4);//fmt
	input.read(chars, 4);//subchunk 1 size
	input.read(chars, 2);//PCM
	if ((bytesToEndian(chars, false) & 0x0000ffff) != 1)
	{
		audio_throw_error("wave file cannot be read");
		return;
	}
	input.read(chars, 2);//channels
	buffer->channelCount = bytesToEndian(chars, false) & 0x0000ffff;
	input.read(chars, 4);//sampleRate
	buffer->sampleRate = bytesToEndian(chars, false);
	input.read(chars, 4);//byteRate
	input.read(chars, 2);//blockAllign
	input.read(chars, 2);//bitsPerSample
	const int bytes_per_sample = (bytesToEndian(chars, false) & 0x0000ffff) >> 3;
	const int bitshift = 16 - (bytesToEndian(chars, false) & 0x0000ffff);//shift samples left
	input.read(chars, 4);//data
	if (strncmp(chars, "data", 4) != 0)
	{
		input.seekg(input.beg + 38);
		input.read(chars, 4);//data
		if (strncmp(chars, "data", 4) != 0)
		{
			audio_throw_error("the file is either written in an untypical way or corrupted");
			return;
		}
	}

	input.read(chars, 4);

	const uint num_bytes = bytesToEndian(chars, false);
	buffer->sampleCount = num_bytes >> (1 - bitshift);
#if (_DEBUG) 
	std::cout << "loading file..." << '\n';
#endif
	char* data = new char[num_bytes];
	input.read(data, num_bytes);
	if (!bitshift) buffer->samples = (i16*)data;
	else {
		if (bitshift == 8) {
			if (buffer->samples) delete[] buffer->samples;
			buffer->samples = new i16[num_bytes];
			for (int i = 0; i < num_bytes; ++i) {
				buffer->samples[i] = (i16)((unsigned char)data[i] << bitshift);
			}
			delete[] data;
		}
		else if (bitshift == -16) {
			if (buffer->samples) delete[] buffer->samples;
			int* ints = (int*)data;
			buffer->samples = new i16[num_bytes / 4];
			for (int i = 0; i < num_bytes; i += 4) {
				buffer->samples[i >> 2] = data[i + 2] | data[i + 3] << 8;
			}
			delete[] data;
		}
		else {
			audio_throw_error("unsupported bitrate" << '\n');
			return;
		}
	}

	input.close();
}

void fau::loadOGGFile(const std::string name, Buffer* buffer) {
	/*OggVorbis_File vf;
	FILE* file;
	fopen_s(&file, name.data(), "rb");
	if (file == NULL) {
		audio_throw_error("failed to open file");
		return;
	}
	if (ov_open_callbacks(file, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0) {
		audio_throw_error("the file does not appear to be in an ogg bitstream");
		return;
	}
	vorbis_info* vi = ov_info(&vf, -1);
	buffer->channelCount = vi->rate;
	if (buffer->channelCount > 2) {
		audio_throw_error("only up to two channels are supported");
		return;
	}
	buffer->sampleRate = vi->rate;
	const unsigned int sampleCount = ov_pcm_total(&vf, -1) * buffer->channelCount;
	
	//4kB
	#define size 4096
	char* pcm = new char[sampleCount * 2];
	buffer->samples = new short[size / 2];
	int current_section;
	for (uint i = 0; i < sampleCount; i += 4096) {
		ov_read(&vf, pcm + i, size - i, 0, 2, 1, &current_section);
	}
	buffer->samples = (short*)pcm;*/

	OggBuffer buf = oggLoadFile(name.data());
	if (buffer->samples) delete[] buffer->samples;
	buffer->samples = buf.samples;
	buffer->sampleCount = buf.sampleCount;
	buffer->sampleRate = buf.sampleRate;
	buffer->channelCount = buf.channelCount;
}

fau::LoadingStreamWAV::LoadingStreamWAV(const std::string& source, const uint loadCount) {
	LoadingStreamWAV::loadCount = loadCount;
	input.open(source, std::ios::binary);
	if (!input.is_open()) {
		audio_throw_error("failed to open file");
		return;
	}
	char chars[4];
	input.read(chars, 4); //chunkid
	if (strncmp(chars, "RIFF", 4) != 0)
	{
		audio_throw_error("this is not a valid WAVE file");
		return;
	}
	input.read(chars, 4);//chunksize
	input.read(chars, 4);//format
	if (strncmp(chars, "WAVE", 4) != 0)
	{
		audio_throw_error("this is not a valid WAVE file");
		return;
	}
	input.read(chars, 4);//fmt
	input.read(chars, 4);//subchunk 1 size
	input.read(chars, 2);//PCM
	if ((bytesToEndian(chars, false) & 0x0000ffff) != 1)
	{
		audio_throw_error("wave file cannot be read");
		return;
	}
	input.read(chars, 2);//channels
	channelCount = bytesToEndian(chars, false) & 0x0000ffff;
	input.read(chars, 4);//sampleRate
	sampleRate = bytesToEndian(chars, false);
	input.read(chars, 4);//byteRate
	input.read(chars, 2);//blockAllign
	input.read(chars, 2);//bitsPerSample
	const int bytes_per_sample = (bytesToEndian(chars, false) & 0x0000ffff) >> 3;
	bitshift = 16 - (bytesToEndian(chars, false) & 0x0000ffff);//shift samples left
	input.read(chars, 4);//data
	input.read(chars, 4);

	const uint num_bytes = bytesToEndian(chars, false);
	sampleCount = num_bytes / 2;

	eof = false;
}

short* fau::LoadingStreamWAV::retrieveSamples(const uint sample) {
	short* result;
	const int byteCount = sample << (1 + (bitshift / 8));
	input.seekg(44 + byteCount);
	char* data = new char[loadCount << 1];
	input.read(data, loadCount << 1);
	if (input.rdstate() & input.eofbit) {
		retrieve = sampleCount - sample;
		if (sample > sampleCount) retrieve = 0;
		input.clear();
	}
	else {
		if (input.rdstate() & input.failbit) {
			//audio_throw_error("failed to read from stream");
			input.clear();
		}
		eof = false;
		retrieve = loadCount;
	}

	if (!retrieve) eof = true;

	if (bitshift) {
		if (bitshift == 8) {
			result = new i16[loadCount];
			for (int i = 0; i < loadCount; ++i) {
				result[i] = (i16)((unsigned char)data[i] << bitshift);
			}
			delete[] data;
		}
		else if (bitshift == -16) {
			int* ints = (int*)data;
			result = new i16[loadCount];
			for (int i = 0; i < loadCount; i += 4) {
				result[i >> 2] = data[i + 2] | data[i + 3] << 8;
			}
			delete[] data;
		}
		else {
			audio_throw_error("unsupported bitrate" << '\n');
			return new i16[loadCount];
		}
	}
	else {
		result = (i16*)data;
	}
	return result;
}

fau::LoadingStreamWAV::~LoadingStreamWAV() {
	input.close();
}

fau::LoadingStreamOGG::LoadingStreamOGG(const std::string& source, const uint loadCount) {
	LoadingStreamOGG::loadCount = loadCount;
	id = oggStream::openStream(source.data());
	OggBuffer buf = oggStream::getInfo(id);
	sampleCount = buf.sampleCount;
	sampleRate = buf.sampleRate;
	channelCount = buf.channelCount;
}

short* fau::LoadingStreamOGG::retrieveSamples(const uint sample) {
	i16* result = new i16[loadCount];
	retrieve = oggStream::loadSamples(result, loadCount, sample, id, channelCount) * channelCount;
	eof = false;
	if (retrieve == 0) eof = true;
	return result;
}

fau::LoadingStreamOGG::~LoadingStreamOGG() {
	oggStream::closeStream(id);
}