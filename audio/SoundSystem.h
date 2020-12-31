#pragma once
#include "../logic/Types.h"
#include "Buffer.h"

namespace fau {
	///<summary>
	/// A system for playing sounds easily.
	///</summary>
	struct SFXSystem {
	protected:
		ALuint* sources;
		size source_count;

	public:
		SFXSystem() = default;

		///<summary>
		/// The position of the listener. Free to modify.
		///</summary>
		glm::vec3 listener_position = glm::vec3(0.);

		///<summary>
		/// Construct a SFXSystem.
		///</summary>
		///<param name="source_count">The maximum number of sounds that can play at the same time.</param>
		SFXSystem(size source_count);

		///<summary>
		/// Create all required OpenAL buffers and objects.
		///</summary>
		void init();

		///<summary>
		/// Configure the attenuation.
		///</summary>
		///<param name="min_distace">Beyond this distance, the volume will decrease.</param>
		///<param name="rolloff">The higher, the faster the sound will become quieter with distance.</param>
		void setAttenuationAttributes(float min_distace, float rolloff);

		///<summary>
		/// Play a sound somewhere in space.
		///</summary>
		///<param name="buffer">A buffer containing the sound.</param>
		///<param name="volume">How loud the sound will be.</param>
		///<param name="pitch">The pitch of the sound.</param>
		///<param name="position">The absolute position of the sound.</param>
		void playSound(Buffer& buffer, float volume, float pitch, glm::vec3 position);

		///<summary>
		/// Play a sound right where the listener is.
		///</summary>
		///<param name="buffer">A buffer containing the sound.</param>
		///<param name="volume">How loud the sound will be.</param>
		///<param name="pitch">The pitch of the sound.</param>
		void playSound(Buffer& buffer, float volume, float pitch);

		///<summary>
		/// Destroy all allocated data.
		///</summary>
		void dispose();
	};
}