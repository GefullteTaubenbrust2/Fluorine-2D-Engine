#pragma once
#include "Matrices.h"
#include <vector>

namespace flo {
	struct SpriteSheet {
		int width, height;
		std::vector<glm::ivec4> allocators;
		std::vector<glm::ivec4> allocated;
		std::vector<int> free_queue;
		int maximum_width, maximum_height;

		SpriteSheet() = default;

		SpriteSheet(int width, int height);

		glm::ivec2 allocate(glm::ivec2 rect);

		void free(std::vector<int>& allocation_ids);

		void free(glm::ivec4 space);

	private:
		void clean();

		void split_around(glm::ivec4 around);
	};
}