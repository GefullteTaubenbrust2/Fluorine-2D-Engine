#include "SpriteSheet.h"
#include <iostream>

namespace flo {
	glm::ivec4 new_split[4];

	int imax(int a, int b) {
		return a > b ? a : b;
	}

	int imin(int a, int b) {
		return a < b ? a : b;
	}

	int split(glm::ivec4 to_split, glm::ivec4 splitter) {
		glm::ivec4 intersection = glm::ivec4(imax(to_split.x, splitter.x), imax(to_split.y, splitter.y), imin(to_split.z, splitter.z), imin(to_split.w, splitter.w));

		int left = -to_split.x + intersection.x;
		int right = to_split.z - intersection.z;
		int top = -to_split.y + intersection.y;
		int bottom = to_split.w - intersection.w;

		int index = 0;
		if (left > 1) {
			new_split[index] = glm::vec4(to_split.x, to_split.y, intersection.x, to_split.w);
			++index;
		}
		if (right > 1) {
			new_split[index] = glm::vec4(intersection.z, to_split.y, to_split.z, to_split.w);
			++index;
		}
		if (top > 1) {
			new_split[index] = glm::vec4(to_split.x, to_split.y, to_split.z, intersection.y);
			++index;
		}
		if (bottom > 1) {
			new_split[index] = glm::vec4(to_split.x, intersection.w, to_split.z, to_split.w);
			++index;
		}

		return index;
	}

	SpriteSheet::SpriteSheet(int width, int height) : 
		width(width), height(height), maximum_width(width), maximum_height(height) {
		allocators.push_back(glm::ivec4(0, 0, width, height));
	}

	glm::ivec4 getIntersection(glm::ivec4& a, glm::ivec4& b) {
		return glm::ivec4(imax(a.x, b.x), imax(a.y, b.y), imin(a.z, b.z), imin(a.w, b.w));
	}

	bool intersection(glm::ivec4& a, glm::ivec4& b) {
		glm::ivec4 intersection = glm::ivec4(imax(a.x, b.x), imax(a.y, b.y), imin(a.z, b.z), imin(a.w, b.w));
		return intersection.z - intersection.x > 0 && intersection.w - intersection.y > 0;
	}

	//If a is inside b, 1 is added. If b is inside a, 2 is added. Default value is 0.
	int inside(glm::ivec4& a, glm::ivec4& b) {
		return (a.x >= b.x && a.y >= b.y && a.z <= b.z && a.w <= b.w) + (b.x >= a.x && b.y >= a.y && b.z <= a.z && b.w <= a.w) * 2;
	}

	bool fits(glm::ivec4 rect, glm::ivec4 into) {
		return (rect.x >= into.x && rect.y >= into.y && rect.z < into.z && rect.w < into.w);
	}

	u32 SpriteSheet::allocate(glm::ivec2 rect) {
		if (rect.x > maximum_width || rect.y > maximum_height) return INVALID_INDEX;

		//Check all possible candidates
		glm::ivec4 allocator_fit = glm::ivec4(-1);
		for (int i = 0; i < allocators.size(); ++i) {
			glm::ivec4 al = allocators[i];
			glm::ivec4 check = glm::ivec4(al.x, al.y, rect.x + al.x, rect.y + al.y);
			if (inside(check, al) & 1) {
				allocator_fit = allocators[i];
			}
		}
		if (allocator_fit.x < 0) return INVALID_INDEX;
		glm::ivec4 allocation = glm::ivec4(allocator_fit.x, allocator_fit.y, allocator_fit.x + rect.x, allocator_fit.y + rect.y);

		split_around(allocation);

		++current_hash;

		allocated.insert(std::make_pair(current_hash, allocation));

		return current_hash;
	}

	glm::ivec4 SpriteSheet::getBounds(u32 hash)
	{
		auto iter = allocated.find(hash);
		if (iter == allocated.end()) return glm::ivec4(-1);
		return iter->second;
	}

	void SpriteSheet::split_around(glm::ivec4 around) {
		std::vector<glm::ivec4> intersections;
		for (int i = 0; i < allocators.size(); ++i) {
			glm::ivec4 al = allocators[i];
			if (intersection(al, around)) {
				intersections.push_back(al);
				allocators.erase(allocators.begin() + i);
				--i;
			}
		}

		for (int i = 0; i < intersections.size(); ++i) {
			int add = split(intersections[i], around);
			for (int j = 0; j < add; ++j) {
				allocators.push_back(new_split[j]);
			}
		}

		clean();
	}

	void SpriteSheet::clean() {
		maximum_width = 0;
		maximum_height = 0;
		for (int i = 0; i < allocators.size(); ++i) {
			glm::ivec4 al = allocators[i];
			if (al.z - al.x > maximum_width) maximum_width = al.z - al.x;
			if (al.w - al.y > maximum_height) maximum_height = al.w - al.y;
			for (int j = 0; j < allocators.size(); ++j) {
				glm::ivec4 al2 = allocators[j];
				if (i == j) continue;
				if (inside(al, al2) & 1) {
					allocators.erase(allocators.begin() + i);
					--i;
					break;
				}
			}
		}
	}

	void SpriteSheet::free() {
		allocators.clear();
		allocators.push_back(glm::ivec4(0, 0, width, height));

		for (int i = 0; i < allocated.size(); ++i) {
			split_around(allocated[i]);
		}
		clean();
	}

	void SpriteSheet::free(u32 hash) {
		allocated.erase(hash);
		++free_queue;
		if (free_queue > 10) {
			free();
			free_queue = 0;
		}
	}
}