#pragma once
#include <al.h>
#include <alc.h>

#include "AudioSetup.h"
#include "Buffer.h"
#include "../logic/MathUtil.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fau {
	/// <summary> A struct for playing the sounds stored within buffers </summary>
	/// \see Buffer
	struct BufferedPlayer {
	protected:
		double time = 0;
		PlayerState state = PlayerState::stopped;

		ALsizei sampleRate;
		ALboolean loop = false;
		uint source = 0;
		bool source_loaded = false;

	public:
		/// <summary> WARNING: This variable is read-only</summary>
		Buffer* buffer = nullptr;

		/// <summary> Construct a player using a buffer
		/// WARNING: The player may not be able to be constructed, if the total number of sources exceeds the max (1 source is created by this player)
		/// WARNING: The buffer is never unallocated from this struct
		/// </summary>
		/// <param name="buffer"> The buffer to be played</param>
		/// \see mono_sources
		/// \see stereo_sources
		BufferedPlayer(Buffer* buffer);

		BufferedPlayer() = default;

		void operator=(const BufferedPlayer& bp);

		/// <summary> Continues the player if paused and resets the player if stopped or playing </summary>
		void play();

		/// <summary> Pauses the player </summary>
		void pause();

		/// <summary> Stops the player </summary>
		void stop();

		/// <summary> Is used for en-/disables looping the sound </summary>
		/// <param name="loop"> Determines if looping should be en- or disabled </param>
		void setLoop(const bool loop);

		/// <summary> Is used to check if the player is looping or not </summary>
		/// <returns> The bool for determining if the player is looping </returns>
		bool getLoop() const;

		/// <summary> Set the timestamp of the sound to continue playing from </summary>
		/// <param name="t"> The timestamp in seconds </param>
		void setPlayingOffset(const double t);

		/// <summary> Get the timestamp at which the player currently is </summary>
		/// <returns> The current timestamp </returns>
		double getPlayingOffset() const;

		/// <summary> Set the volume of the player </summary>
		/// <param name="vol"> The volume, 1 corresponding to 100% your device's volume </param>
		void setVolume(const float vol);

		/// <summary> Get the volume of the player </summary>
		/// <returns> The volume, 1 corresponding to 100% your device's volume </returns>
		float getVolume() const;

		/// <summary> Set the player's pitch, meaning how fast it plays the samples </summary>
		/// <param name="pitch"> The pitch, 1 being default </param>
		void setPitch(const float pitch);

		/// <summary> Get the player's pitch, meaning how fast it plays the samples </summary>
		/// <returns> The player's pitch, 1 being default </returns>
		float getPitch() const;

		/// <summary> Get the state of the player </summary>
		/// <returns> The state (playing/stopped/paused) </returns>
		PlayerState getState();

		/// <summary> Set the position of the source relative to the listener </summary>
		/// <param name="pos"> The position as a 3D vector </param>
		void setPosition(const glm::vec3& pos);

		/// <summary> Get the position of the source relative to the listener </summary>
		/// <returns> The position as a 3D vector </returns>
		glm::vec3 getPosition() const;

		/// <summary> Set the velocity of the source relative to the listener </summary>
		/// <param name="pos"> The velocity as a 3D vector </param>
		void setVelocity(const glm::vec3& pos);

		/// <summary> Get the velocity of the source relative to the listener </summary>
		/// <returns> The velocity as a 3D vector </returns>
		glm::vec3 getVelocity() const;

		/// <summary> The destructor </summary>
		void dispose();
	};
}