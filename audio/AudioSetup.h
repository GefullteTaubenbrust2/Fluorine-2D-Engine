#pragma once
#include <al.h>
#include <alc.h>
#include <vector>
#include <iostream>
#include "../logic/Types.h"

namespace fau {
	/// <summary> 
	/// A pointer to the device used for audio output
	/// </summary>
	//extern ALCdevice* device;
	
	/// <summary>  
	/// The number of mono sources guaranteed to be supported
	/// </summary>
	extern ALint mono_sources;

	/// <summary>  
	/// The number of stereo sources guaranteed to be supported
	/// </summary>
	extern ALint stereo_sources;

	/// <summary>  
	/// A vector containing all device names
	/// </summary>
	extern std::vector<std::string> output_devices, input_devices;

	/// <summary>  
	/// This function will init the vector above
	/// </summary>
	void listDevices();

	/// <summary>  
	/// An enum for the possible states of a player
	/// </summary>
	enum PlayerState {
		stopped = 0,
		paused = 1,
		playing = 2
	};

	/// <summary>  
	/// Initialize openAL, must be called before any players, recorders or buffers are used
	/// </summary>
	/// <param name="device_index"> The index of the device in the device index returned by <c>listDevices()</c></param>
	void init(const int device_index = -1);

	/// <summary>  
	/// Checks how many sources are guaranteed to be supported and stores the variables in the variables mono_sources and stereo_sources
	/// </summary>
	/// \see mono_sources
	/// \see stereo_sources
	void setMaxSources();

	/// <summary>  
	/// Dispose of the openAl context, should be called at the end of your program
	/// </summary>
	void dispose();

	/// <summary>  
	/// Apply a reverb filter to a set of samples
	/// </summary>
	/// <param name="samples"> The samples to apply the effect to</param>
	/// <param name="sampleCount"> The amount of samples in samples</param>
	/// <param name="sampleRate"> The desired sample rate (giving the value of a e.g. a buffer is not always neccessary, meaning you can mess around with this value within reason)</param>
	/// <param name="channelCount"> The channelCount of the samples (giving the value of e.g. a buffer is not always neccessary, meaning you can mess around with this value within reason)</param>
	/// <param name="delay_millis"> The delay for the reverb in milliseconds (the longer the delay, the longer it takes until echoes are heard)</param>
	/// <param name="decay_f"> The decay constant for the reverb (The higher, the faster the echoes will fade away)</param>
	void reverberate(i16* samples, const unsigned int sampleCount, const unsigned int sampleRate, const unsigned int channelCount, const float delay_millis, const float decay_f);

	struct Buffer;
	void reverberate(Buffer& buffer, const float delay_millis, const float decay_f);
}