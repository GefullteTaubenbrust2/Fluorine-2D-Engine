#pragma once
#include "Buffer.h"

namespace fau {
	/// <summary> Write the contents of a buffer into a .wav file
	/// WARNING: The buffer is not unallocated from this function
	/// </summary>
	/// <param name="filename"> The path and name of the file </param>
	/// <param name="buffer A"> pointer the buffer to be written from </param>
	void outputWAVFile(const std::string filename, Buffer* buffer);
}