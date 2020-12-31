#pragma once
#include "Matrices.h"
#include <vector>
#include <map>
#include "Types.h"

namespace flo {
	///<summary>
	/// A struct for allocating rectangles to a larger rectangle.
	///</summary>
	struct SpriteSheet {
		///<summary>
		/// The width of the given space. WARNING: READ-ONLY!
		///</summary>
		int width, height;

		///<summary>
		/// All free space. WARNING: READ-ONLY!
		///</summary>
		std::vector<glm::ivec4> allocators;

		///<summary>
		/// The bounds of the allocated rectangles. WARNING: READ-ONLY!
		///</summary>
		std::map<u32, glm::ivec4> allocated;

		///<summary>
		/// How many rectangles are waiting to be freed? WARNING: READ-ONLY!
		///</summary>
		int free_queue = 0;

		///<summary>
		/// The largest width and height of spaces availible. WARNING: READ-ONLY!
		///</summary>
		int maximum_width, maximum_height;

		///<summary>
		/// The hash last created. WARNING: READ-ONLY!
		///</summary>
		u32 current_hash = 0;

		SpriteSheet() = default;

		///<summary>
		/// Construct a SpriteSheet with a width and a height.
		///</summary>
		///<param name="width">The width of the availible space. Must be positive.</param>
		///<param name="height">The height of the availible space. Must be positive.</param>
		SpriteSheet(int width, int height);

		///<summary>
		/// Allocate a rectangle to the space.
		///</summary>
		///<param name="rect">The width and height of the rectangle.</param>
		///<returns>The hash of the bounds allocated if successfull, INVALID_INDEX otherwhise.</returns>
		u32 allocate(glm::ivec2 rect);

		///<summary>
		/// Get the bounds of a rectangle.
		///</summary>
		///<param name="hash">The hash of the rectangle in question.</param>
		///<returns>The bounds if successfull, [-1, -1, -1, -1] otherwhise.</returns>
		glm::ivec4 getBounds(u32 hash);

		///<summary>
		/// Free a rectangle.
		///</summary>
		///<param name="hash">The hash of the rectangle in question.</param>
		void free(u32 hash);

	private:
		void free();

		void clean();

		void split_around(glm::ivec4 around);
	};
}