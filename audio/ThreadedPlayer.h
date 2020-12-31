#pragma once
#include "FileLoader.h"
#include "BufferedPlayer.h"
#include "AudioSetup.h"
#include "AErrorHandler.h"

#include <thread>
#include <al.h>
#include <alc.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fau {
	/// <summary> A struct for playing from streams
	/// WARNING: The member variables are read-only (except for loop)
	/// </summary>
	/// \see ThreadedStream
	struct ThreadedPlayer {
	protected:
		PlayerState state = PlayerState::stopped;
		std::thread* thread = nullptr;
		ALuint source;
		int bufferid;
		bool buffers_cleared = true;
		bool buffer0 = true;
		ALuint buffers[2];
		bool req_pause = false, req_stop = false;
		uint req_id = 0;
		ALfloat req_time = 0;
		bool q0, q1;

		void run();

	public:
		ThreadedStream* stream;

		/// <summary> Should the player loop its sound? (This variable can be changed without risk)</summary>
		bool loop = false;

		/// <summary> Construct a ThreadedPlayer
		/// WARNING: The player may not be able to be constructed, if the total number of sources exceeds the max (1 source is created by this player)
		/// </summary>
		/// <param name="stream"> The stream to play (you should keep a pointer to it outside of this constructor)</param>
		/// \see mono_sources
		/// \see stereo_sources
		ThreadedPlayer(ThreadedStream* stream);

		ThreadedPlayer() = default;

		/// <summary> Continues the player if paused and resets the player if stopped or playing</summary>
		void play();

		/// <summary> Stops the player
		/// WARNING: Only affects the value of getPlayingOffset() immediately
		/// </summary>
		/// \see getPlayingOffset
		void stop();
		
		/// <summary> Pauses the player</summary>
		void pause();

		/// <summary> Get the state of the player</summary>
		/// <returns> The state (playing/stopped/paused)</returns>
		PlayerState getState();

		/// <summary> Get the timestamp at which the player currently is</summary>
		/// <returns> The current timestamp</returns>
		double getPlayingOffset();

		/// <summary> Set the timestamp of the sound to continue playing from
		/// WARNING: Only affects the value of getPlayingOffset() if the player is played after this function is called or already playing
		/// </summary>
		/// <param name="t"> The timestamp in seconds</param>
		/// \see getPlayingOffset
		void setPlayingOffset(const double seconds);

		/// <summary> Set the volume of the player</summary>
		/// <param name="vol"> The volume, 1 corresponding to 100% your device's volume</param>
		void setVolume(const float vol);

		/// <summary> Get the volume of the player</summary>
		/// <returns> The volume, 1 corresponding to 100% your device's volume</returns>
		float getVolume();

		/// <summary> Set the player's pitch, meaning how fast it plays the samples</summary>
		/// <param name="pitch"> The pitch, 1 being default</param>
		void setPitch(const float pitch);

		/// <summary> Get the player's pitch, meaning how fast it plays the samples</summary>
		/// <returns> The player's pitch, 1 being default</returns>
		float getPitch();

		/// <summary> Set the position of the source relative to the listener</summary>
		/// <param name="pos"> The position as a 3D vector</param>
		void setPosition(const glm::vec3& pos);

		/// <summary> Get the position of the source relative to the listener</summary>
		/// <returns> The position as a 3D vector</returns>
		glm::vec3 getPosition();

		/// <summary> Set the velocity of the source relative to the listener</summary>
		/// <param name="pos"> The velocity as a 3D vector</param>
		void setVelocity(const glm::vec3& pos);

		/// <summary> Get the velocity of the source relative to the listener</summary>
		/// <returns> The velocity as a 3D vector</returns>
		glm::vec3 getVelocity();

		/// <summary> The destructor</summary>
		void dispose();
	};
}