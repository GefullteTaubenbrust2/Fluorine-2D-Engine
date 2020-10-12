#pragma once
#include <iostream>
#include <al.h>
#include <alc.h>

namespace fau {
	/// <summary> A function to call if an error occurs </summary>
	void setError();

	/// <summary> This is true from the moment an error occurs in the library's files </summary>
	bool errorThrown();
	
	//extern void(*error_event)();
}

#if _DEBUG
#define audio_throw_error(error) setError(); std::cerr << error << " has occured in line " << __LINE__ << " of the file " << __FILE__ << '\n'
#define audio_al_call(call) call; if (alGetError() != AL_NO_ERROR) { audio_throw_error("An openAL error"); }
#else
#define audio_al_call(call) call
#define audio_throw_error(error) std::cerr << error << '\n'
#endif