#include "Tilemap.h"

#include "../graphics/Window.h"

#include "../logic/Sort.h"

namespace pixelgame {
#if 1
	int divideFixed(const int a, const int b) {
		if (a >= 0) return a / b;
		else return (a - b + 1) / b;
	}

	int modFixed(const int a, const int b) {
		if (a >= 0) return a % b;
		else return b - 1 + (a + 1) % b;
	}
#elif 0
	int divideFixed(const int a, const int b) {
		const char c = a < 0;
		return (a - (b - 1) * c) / b;
	}

	int modFixed(const int a, const int b) {
		const char c = a < 0;
		return (b - 1) * c + (a + c) % b;
	}
#else
	int divideFixed(const int a, const int b) {
		const char c = ((unsigned int)(a & (1 << 31)) >> 31);
		return (a - (b - 1) * c) / b;
	}

	int modFixed(const int a, const int b) {
		const char c = ((unsigned int)(a & (1 << 31)) >> 31);
		return (b - 1) * c + (a + c) % b;
	}
#endif

	fgr::ArrayTexture tilemap;

	void initTileset() {
		tilemap = fgr::ArrayTexture(512, 512, 1, 4);
		//tilemap.loadFromFile("res/tileset.png", 0);
		tilemap.loadFromFile("res/bgj/tileset.png", 0);
		tilemap.createBuffer(GL_REPEAT, GL_NEAREST);
	}

	/*SeamlessTile::SeamlessTile(glm::vec4 bounds, int layer, int priority, bool inside) : priority(priority), inside(inside) {
		for (int i = 0; i < 19; ++i) {
			sprites[i] = graphics::Sprite(layer, glm::mat3(1.0), glm::vec4(bounds.x + (float)i / 19. * (bounds.z - bounds.x), bounds.y, bounds.x + (float)(i + 1) / 19 * (bounds.z - bounds.x), bounds.a), glm::vec4(1.0));
		}
	}*/

	const int Tile::generation_limit = 16;

	Tile::Tile(glm::vec4 bounds_full, glm::vec4 bounds_partial, int layer, int priority, bool connect_to_higher) : connect_to_higher(connect_to_higher), priority(priority) {
		sprites[0] = fgr::Sprite(layer, glm::mat3(1.0), glm::vec4(bounds_full), glm::vec4(1.0));

		for (int i = 0; i < 12; ++i) {
			const float x = bounds_partial.x + (bounds_partial.z - bounds_partial.x) * (float)i / 12.f;
			sprites[i + 1] = fgr::Sprite(layer, glm::mat3(1.0), glm::vec4(x, bounds_partial.y, x + (bounds_partial.z - bounds_partial.x) / 12.f, bounds_partial.a), glm::vec4(1.0));
		}
	}

	Tile::Tile(glm::vec4 bounds, int layer, int priority, bool connect_to_higher) : connect_to_higher(connect_to_higher), priority(priority) {
		glm::vec4 bounds_full = glm::vec4(bounds.x, bounds.y, bounds.x + (bounds.z - bounds.x) / 13.f, bounds.a);

		sprites[0] = fgr::Sprite(layer, glm::mat3(1.0), glm::vec4(bounds_full), glm::vec4(1.0));


		for (int i = 1; i < 13; ++i) {
			const float x = bounds.x + (bounds.z - bounds.x) * (float)i / 13.f;
			sprites[i] = fgr::Sprite(layer, glm::mat3(1.0), glm::vec4(x, bounds.y, x + (bounds.z - bounds.x) / 13.f, bounds.a), glm::vec4(1.0));
		}
	}

	Tile::Tile(glm::vec4 bounds_full, glm::vec4 bounds_partial, int layer, int priority, bool connect_to_higher, bool collision) : connect_to_higher(connect_to_higher), priority(priority), collision(collision) {
		sprites[0] = fgr::Sprite(layer, glm::mat3(1.0), glm::vec4(bounds_full), glm::vec4(1.0));

		for (int i = 0; i < 12; ++i) {
			const float x = bounds_partial.x + (bounds_partial.z - bounds_partial.x) * (float)i / 12.f;
			sprites[i + 1] = fgr::Sprite(layer, glm::mat3(1.0), glm::vec4(x, bounds_partial.y, x + (bounds_partial.z - bounds_partial.x) / 12.f, bounds_partial.a), glm::vec4(1.0));
		}
	}
	
	const Tile tile_types[20] = {
		//null 0
		Tile(glm::vec4(0.0), glm::vec4(0.0), 0, 0, false),
		
		
		//grass 1
		/*Tile(glm::vec4(0, 512 - 0, 16, 512 - 16) / 512.f, glm::vec4(16, 512 - 16, 208, 512 - 32) / 512.f, 0, 4, true),
		//grass1 2
		Tile(glm::vec4(0, 512 - 16, 16, 512 - 32) / 512.f, glm::vec4(16, 512 - 16, 208, 512 - 32) / 512.f, 0, 4, true),
		//grass2 3
		Tile(glm::vec4(0, 512 - 32, 16, 512 - 48) / 512.f, glm::vec4(16, 512 - 16, 208, 512 - 32) / 512.f, 0, 4, true),
		//grass3 4
		Tile(glm::vec4(16, 512 - 32, 32, 512 - 48) / 512.f, glm::vec4(16, 512 - 16, 208, 512 - 32) / 512.f, 0, 4, true),
		//grass_elevated 5
		Tile(glm::vec4(0, 512 - 0, 16, 512 - 16) / 512.f, glm::vec4(16, 512 - 0, 208, 512 - 16) / 512.f, 0, 5, true),
		//grass1_elevated 6
		Tile(glm::vec4(0, 512 - 16, 16, 512 - 32) / 512.f, glm::vec4(16, 512 - 0, 208, 512 - 16) / 512.f, 0, 5, true),
		//grass2_elevated 7
		Tile(glm::vec4(0, 512 - 32, 16, 512 - 48) / 512.f, glm::vec4(16, 512 - 0, 208, 512 - 16) / 512.f, 0, 5, true),
		//grass3_elevated 8
		Tile(glm::vec4(16, 512 - 32, 32, 512 - 48) / 512.f, glm::vec4(16, 512 - 0, 208, 512 - 16) / 512.f, 0, 5, true),
		//sand 9
		Tile(glm::vec4(0, 512 - 48, 16, 512 - 64) / 512.f, glm::vec4(16, 512 - 48, 208, 512 - 64) / 512.f, 0, 3, true),
		//sand1 10
		Tile(glm::vec4(0, 512 - 64, 16, 512 - 80) / 512.f, glm::vec4(16, 512 - 48, 208, 512 - 64) / 512.f, 0, 3, true),
		//sand2 11
		Tile(glm::vec4(16, 512 - 64, 32, 512 - 80) / 512.f, glm::vec4(16, 512 - 48, 208, 512 - 64) / 512.f, 0, 3, true),
		//sand3 12
		Tile(glm::vec4(32, 512 - 64, 48, 512 - 80) / 512.f, glm::vec4(16, 512 - 48, 208, 512 - 64) / 512.f, 0, 3, true),
		//water 13
		Tile(glm::vec4(0, 512 - 256, 16, 512 - 272) / 512.f, glm::vec4(16, 512 - 256, 208, 512 - 272) / 512.f, 0, 1, false),
		//smooth stone 14
		Tile(glm::vec4(0, 512 - 272, 16, 512 - 288) / 512.f, glm::vec4(16, 512 - 272, 208, 512 - 288) / 512.f, 0, 2, true),*/
		
		//black 1
		Tile(glm::vec4(0, 512 - 16, 16, 512 - 32) / 512.f, glm::vec4(16, 512 - 16, 208, 512 - 32) / 512.f, 0, 14, true, true),
		//blocked 2
		Tile(glm::vec4(0, 512 - 144, 16, 512 - 160) / 512.f, glm::vec4(16, 512 - 144, 208, 512 - 160) / 512.f, 0, 1, true, true),
		//black2 3
		Tile(glm::vec4(0, 512 - 32, 16, 512 - 48) / 512.f, glm::vec4(16, 512 - 32, 208, 512 - 48) / 512.f, 0, 15, true, true),
		//black3 4
		Tile(glm::vec4(0, 512 - 48, 16, 512 - 64) / 512.f, glm::vec4(16, 512 - 48, 208, 512 - 64) / 512.f, 0, 13, true, true),
		//deflector_straight 5
		Tile(glm::vec4(0, 512 - 128, 16, 512 - 144) / 512.f, glm::vec4(16, 512 - 128, 208, 512 - 144) / 512.f, 0, 12, true, true),
		//deflector_diag_0 6
		Tile(glm::vec4(0, 512 - 64, 16, 512 - 80) / 512.f, glm::vec4(16, 512 - 64, 208, 512 - 80) / 512.f, 0, 10, false, true),
		//deflector_diag_1 7
		Tile(glm::vec4(0, 512 - 80, 16, 512 - 96) / 512.f, glm::vec4(16, 512 - 80, 208, 512 - 96) / 512.f, 0, 10, false, true),
		//deflector_diag_2 8
		Tile(glm::vec4(0, 512 - 96, 16, 512 - 112) / 512.f, glm::vec4(16, 512 - 96, 208, 512 - 112) / 512.f, 0, 10, false, true),
		//deflector_diag_3 9
		Tile(glm::vec4(0, 512 - 112, 16, 512 - 128) / 512.f, glm::vec4(16, 512 - 112, 208, 512 - 128) / 512.f, 0, 10, false, true),
		//bg_tile_standard 10,
		Tile(glm::vec4(0, 512 - 144, 16, 512 - 160) / 512.f, glm::vec4(16, 512 - 144, 208, 512 - 160) / 512.f, 0, 1, true, false),
		//bg_tile_rombus0 11,
		Tile(glm::vec4(0, 512 - 160, 16, 512 - 176) / 512.f, glm::vec4(16, 512 - 160, 208, 512 - 176) / 512.f, 0, 3, false, false),
		//bg_tile_rombus0s 12,
		Tile(glm::vec4(0, 512 - 176, 16, 512 - 192) / 512.f, glm::vec4(16, 512 - 176, 208, 512 - 192) / 512.f, 0, 4, false, false),
		//bg_tile_weld 13,
		Tile(glm::vec4(0, 512 - 192, 16, 512 - 208) / 512.f, glm::vec4(16, 512 - 192, 208, 512 - 208) / 512.f, 0, 5, true, false),
		//goal 14,
		Tile(glm::vec4(0, 512 - 208, 16, 512 - 224) / 512.f, glm::vec4(16, 512 - 208, 208, 512 - 224) / 512.f, 0, 7, true, false),
		//abyss 15,
		Tile(glm::vec4(0, 512 - 224, 16, 512 - 240) / 512.f, glm::vec4(16, 512 - 224, 208, 512 - 240) / 512.f, 0, 9, true, false),
		//bg_tile_rombus1 16
		Tile(glm::vec4(0, 512 - 240, 16, 512 - 256) / 512.f, glm::vec4(16, 512 - 240, 208, 512 - 256) / 512.f, 0, 3, false, false),
	};

	/*const SeamlessTile tile_borders[10] = {
		//grass
		SeamlessTile(glm::vec4(16, 512 - 16, 320, 512 - 32) / 512.f, 0, 48),
		//elevated grass
		SeamlessTile(glm::vec4(16, 512 - 0, 320, 512 - 16) / 512.f, 0, 64),
		//sand
		SeamlessTile(glm::vec4(16, 512 - 48, 320, 512 - 64) / 512.f, 0, 32),
		//smooth stone
		SeamlessTile(glm::vec4(16, 512 - 272, 320, 512 - 288) / 512.f, 0, 16, true),
	};*/

	Chunk::Chunk(int x, int y, int size, short(*generation)(int x, int y), InfiniteTileHandler* parent) : x(x), y(y), size(size), tilecount(size*size), parent(parent) {
		tiles = new unsigned short[tilecount * 2];
		for (int x = 0; x < size; ++x) {
			for (int y = 0; y < size; ++y) {
				tiles[x + y * size] = generation(x + Chunk::x * size, y + Chunk::y * size);
				tiles[x + y * size + tilecount] = 1 << 15;
			}
		}
		remesh_needed = true;
	}

	short Chunk::getTile(int x, int y) {
		if (x < 0 || x >= size || y < 0 || y >= size) {
			if (!parent) return TileName::null;
			return parent->getTile(x + Chunk::x * size, y + Chunk::y * size);
		}
		return tiles[x + y * size];
	}

	void Chunk::setTile(int x, int y, short tile) {
		if (x < 0 || x >= size || y < 0 || y >= size) return;
		remesh_needed = true;
		tiles[x + y * size] = tile;
		if (!parent) return;
		for (int xp = -1; xp <= 1; ++xp) {
			for (int yp = -1; yp <= 1; ++yp) {
				if (x + xp < 0 || x + xp >= size || y + yp < 0 || y + yp >= size) {
					Chunk* c = parent->getChunk(divideFixed(Chunk::x * size + x + xp, size), divideFixed(Chunk::y * size + y + yp, size));
					if (c) c->queueRemesh(modFixed(x + xp, size), modFixed(y + yp, size));
					continue;
				}
				tiles[x + xp + (y + yp) * size + tilecount] |= 1 << 15;
			}
		}
	}

	void Chunk::queueRemesh(int x, int y) {
		if (x < 0 || x >= size || y < 0 || y >= size) return;
		tiles[x + y * size + tilecount] |= 1 << 15;
		remesh_needed = true;
	}

	void Chunk::update() {
		if (!inited) {
			sprites.init();
			sprites.dynamic_allocation = true;
			sprites.texture_array = tilemap.id;
			inited = true;
		}

		if (remesh_needed) remesh();
		if (update_needed) {
			sprites.update();
			update_needed = false;
		}
	}

	void Chunk::unload() {
		sprites.dispose();
		if (tiles) delete[] tiles;
		tiles = nullptr;
	}

	int Chunk::generateMesh(int x, int y, fgr::Sprite* output) {
		short neighbours[4];

		neighbours[0] = glm::max(getTile(x, y), (short)0);
		neighbours[1] = glm::max(getTile(x + 1, y), (short)0);
		neighbours[2] = glm::max(getTile(x, y + 1), (short)0);
		neighbours[3] = glm::max(getTile(x + 1, y + 1), (short)0);

		char output_priorities[Tile::generation_limit] = { 0 };

		short types[4] = { 0 };
		int type_count = 0;
		char configurations[4] = { 0 };

		for (int i = 0; i < 4; ++i) {
			const short id = neighbours[i];
			if (id == TileName::null) continue;

			bool new_type = true;
			for (int j = 0; j < type_count; ++j) {
				if (types[j] == id || tile_types[types[j]].priority == tile_types[id].priority || !id) {
					new_type = false;
					break;
				}
			}
			if (!new_type) continue;
			types[type_count] = id;
			++type_count;
		}

		int write_index = 0;
		for (int i = 0; i < type_count; ++i) {
			const int prio = tile_types[types[i]].priority;
			const bool connects = tile_types[types[i]].connect_to_higher;

			for (int j = 0; j < 4; ++j) {
				const int p = tile_types[neighbours[j]].priority;
				if (p == prio || (p > prio && connects)) configurations[i] |= 1 << j;
			}

			const Tile& tile = tile_types[types[i]];

			switch (configurations[i]) {
				//single corners
			case 0b0001:
				output[write_index] = tile.sprites[7]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0010:
				output[write_index] = tile.sprites[8]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0100:
				output[write_index] = tile.sprites[5]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1000:
				output[write_index] = tile.sprites[6]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
				//single sides
			case 0b0011:
				output[write_index] = tile.sprites[2]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0101:
				output[write_index] = tile.sprites[1]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1010:
				output[write_index] = tile.sprites[4]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1100:
				output[write_index] = tile.sprites[3]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
				//missing corners
			case 0b1110:
				output[write_index] = tile.sprites[11]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1101:
				output[write_index] = tile.sprites[12]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1011:
				output[write_index] = tile.sprites[9]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0111:
				output[write_index] = tile.sprites[10]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
				//two corners
			case 0b0110:
				output[write_index] = tile.sprites[8]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				output[write_index] = tile.sprites[5]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1001:
				output[write_index] = tile.sprites[6]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				output[write_index] = tile.sprites[7]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
				//full
			case 0b1111:
				output[write_index] = tile.sprites[0]; output_priorities[write_index] = prio;
				++write_index; if (write_index >= Tile::generation_limit) return Tile::generation_limit;
				break;
			}
		}

		flo::radixSort(output_priorities, (char*)(output), write_index, sizeof(fgr::Sprite));

		return write_index;
	}

	void Chunk::remesh() {
		fgr::Sprite generate[Tile::generation_limit];

		int index = 0;
		int add = 0;
		for (int y = 0; y < size; ++y) {
			for (int x = 0; x < size; ++x) {
				index = (tiles[x + y * size + tilecount] & 0x7fff) + add;
				bool needs_edit = tiles[x + y * size + tilecount] & (1 << 15);
				tiles[x + y * size + tilecount] = index;

				if (!needs_edit) continue;

				Tile tile = tile_types[getTile(x, y)];

				glm::mat3 offset = flo::scale(flo::translate(glm::mat3(1.0), glm::vec2(x + Chunk::x * size, y + Chunk::y * size)), glm::vec2(TILE_SPRITE_SIZE));

				int generated = 0;

				generated = generateMesh(x, y, generate);

				int actual_count = (x + y * size == tilecount - 1) ? (sprites.sprites.size() - index) : ((tiles[x + y * size + tilecount + 1] & 0x7fff) - index + add);

				add += generated - actual_count;

				for (int i = 0; i < generated; ++i) {
					generate[i].transform = offset;
					//generate[i].color *= glm::vec4(0.3, 0.3, 0.5, 1.0);
				}

				if (generated >= actual_count) {
					for (int i = 0; i < actual_count; ++i) {
						sprites.sprites[i + index] = generate[i];
					}
					if (generated != actual_count) sprites.sprites.insert(sprites.sprites.begin() + index + actual_count, generate + actual_count, generate + generated);
				}
				else {
					for (int i = 0; i < generated; ++i) {
						sprites.sprites[i + index] = generate[i];
					}
					sprites.sprites.erase(sprites.sprites.begin() + index + generated, sprites.sprites.begin() + index + actual_count);
				}
			}
		}

		remesh_needed = false;

		update_needed = true;
	}

	void Chunk::update_neighbour(int relative_x, int relative_y) {
		remesh_needed = true;
		int xmin = 0, xmax = -1, ymin = 0, ymax = -1;
		switch (relative_x) {
		case 0:
			xmin = 0;
			xmax = size - 1;
			break;
		case 1:
			xmin = size - 1;
			xmax = size - 1;
			break;
		case -1:
			xmin = 0;
			xmax = 0;
			break;
		}
		switch (relative_y) {
		case 0:
			ymin = 0;
			ymax = size - 1;
			break;
		case 1:
			ymin = size - 1;
			ymax = size - 1;
			break;
		case -1:
			ymin = 0;
			ymax = 0;
			break;
		}

		for (int x = xmin; x <= xmax; ++x) {
			for (int y = ymin; y <= ymax; ++y) {
				tiles[x + y * size + tilecount] |= 1 << 15;
			}
		}
	}

	void Chunk::render(glm::mat3 transform) {
		sprites.setTransformations(transform);
		sprites.draw(fgr::Shader::sprites_instanced);
	}

	InfiniteTileHandler::InfiniteTileHandler(int chunk_size, short(*generation)(int x, int y), int render_distance) : chunk_size(chunk_size), generation(generation) {
		setRenderDistance(render_distance);
	}

	void InfiniteTileHandler::setRenderDistance(int chunks) {
		chunk_x_count = chunks * 2 + 1;
		if (chunk_indices) delete[] chunk_indices;
		chunk_indices = new int[chunk_x_count* chunk_x_count];

		for (int i = 0; i < chunk_x_count * chunk_x_count; ++i) {
			chunk_indices[i] = -1;
		}
	}

	void InfiniteTileHandler::update(glm::mat3& transformations, glm::mat3& transformations_inverse) {
		glm::vec2 min = transformations_inverse * glm::vec3(-1.0, 1.0, 1.0);
		glm::vec2 max = transformations_inverse * glm::vec3(1.0, -1.0, 1.0);

		const int xa = divideFixed(min.x, TILE_SPRITE_SIZE * chunk_size);
		const int ya = divideFixed(min.y, TILE_SPRITE_SIZE * chunk_size);
		const int xb = divideFixed(max.x, TILE_SPRITE_SIZE * chunk_size);
		const int yb = divideFixed(max.y, TILE_SPRITE_SIZE * chunk_size);

		render_bounds = glm::vec4(xa, ya, xb, yb);

		if (xa < center_offset.x - chunk_x_count / 2 + 1 || ya < center_offset.y - chunk_x_count / 2 + 1 || xb > center_offset.x + chunk_x_count / 2 - 1 || yb > center_offset.y + chunk_x_count / 2 - 1) {
			std::cout << "moving focus to " << (xa + xb) / 2 << ' ' << (ya + yb) / 2 << '\n';
			moveFocus((xa + xb) / 2, (ya + yb) / 2);
		}

		if (thread_finished) {
			thread->join();
			delete thread;
			thread = nullptr;
			thread_finished = false;

			const int x = loading->x;
			const int y = loading->y;

			const int xp = x - center_offset.x + chunk_x_count / 2;
			const int yp = y - center_offset.y + chunk_x_count / 2;

			if (xp >= 0 && yp >= 0 && xp < chunk_x_count && yp < chunk_x_count) {
				const int index = chunks.size();
				chunks.push_back(*loading);
				chunk_indices[xp + yp * chunk_x_count] = index;
				for (int xa = -1; xa <= 1; ++xa) {
					for (int ya = -1; ya <= 1; ++ya) {
						if (xa == 0 && ya == 0) continue;
						Chunk* c = getChunk(xa + x, ya + y);
						if (c) c->update_neighbour(-xa, -ya);
					}
				}
			}

			if (loading) delete loading;
			loading = nullptr;
		}

		for (int i = 0; i < chunks.size(); ++i) {
			chunks[i].update();
		}

		bool brk = false;
		for (int x = render_bounds.x - 1 ; x <= render_bounds.z + 1; ++x) {
			for (int y = render_bounds.y - 1; y <= render_bounds.a + 1; ++y) {
				Chunk* chunk = getChunk(x, y, false);
				if (thread) {
					brk = true;
					break;
				}
				if (!chunk && !thread) {
					std::cout << "queued generation at " << x << ' ' << y << '\n'; 
					queued = glm::ivec2(x, y);
					thread = new std::thread(&InfiniteTileHandler::queueChunk, this, x, y);
					brk = true;
					break;
				}
			}
			if (brk) break;
		}
	}

	
	void InfiniteTileHandler::moveFocus(int x, int y) {
		for (int i = 0; i < chunk_x_count * chunk_x_count; ++i) {
			chunk_indices[i] = -1;
		}

		const int xp = x - chunk_x_count / 2;
		const int yp = y - chunk_x_count / 2;

		center_offset = glm::vec2(x, y);

		for (int i = 0; i < chunks.size(); ++i) {
			Chunk& chunk = chunks[i];
			if (chunk.x >= xp && chunk.y >= yp && chunk.x < xp + chunk_x_count && chunk.y < yp + chunk_x_count) {
				chunk_indices[chunk.x - xp + (chunk.y - yp) * chunk_x_count] = i;
			}
			else {
				chunk.unload();
				chunks.erase(chunks.begin() + i);
				--i;
				continue;
			}
		}
		std::cout << "chunk count: " << chunks.size() << '\n';
	}

	short InfiniteTileHandler::getTile(int x, int y, bool force) {
		Chunk* chunk = getChunk(divideFixed(x, chunk_size), divideFixed(y, chunk_size), force);
		if (!chunk) return -1;
		return chunk->getTile(modFixed(x, chunk_size), modFixed(y, chunk_size));
	}

	bool InfiniteTileHandler::setTile(int x, int y, short tile, bool force) {
		Chunk* chunk = getChunk(divideFixed(x, chunk_size), divideFixed(y, chunk_size), force);
		if (!chunk) return false;
		chunk->setTile(modFixed(x, chunk_size), modFixed(y, chunk_size), tile);
		return true;
	}

	void InfiniteTileHandler::render(glm::mat3& transformations) {
		for (int x = render_bounds.x; x <= render_bounds.z; ++x) {
			for (int y = render_bounds.y; y <= render_bounds.a; ++y) {
				Chunk* chunk = getChunk(x, y, false);
				if (chunk) chunk->render(transformations);
			}
		}
	}

	Chunk* InfiniteTileHandler::createChunk(int x, int y) {
		const int xp = x + center_offset.x - chunk_x_count / 2;
		const int yp = y + center_offset.y - chunk_x_count / 2;

		Chunk* get = getChunk(xp, yp, false);
		if (get) {
			return get;
		}

		std::cout << "creating chunk at " << xp << ' ' << yp << '\n';

		if (x >= 0 && y >= 0 && x < chunk_x_count && y < chunk_x_count) {
			const int index = chunks.size();
			chunks.push_back(Chunk(xp, yp, chunk_size, generation, this));
			chunk_indices[x + y * chunk_x_count] = index;
			for (int xa = -1; xa <= 1; ++xa) {
				for (int ya = -1; ya <= 1; ++ya) {
					if (xa == 0 && ya == 0) continue;
					Chunk* c = getChunk(xa + xp, ya + yp);
					if (c) c->update_neighbour(-xa, -ya);
				}
			}
			return chunks.data() + index;
		}
		return nullptr;
	}

	Chunk* InfiniteTileHandler::getChunk(int x, int y, bool force) {
		const int xp = x - center_offset.x + chunk_x_count / 2;
		const int yp = y - center_offset.y + chunk_x_count / 2;

		if (force) {
			return createChunk(xp, yp);
		}
		else {
			if (xp < 0 || xp >= chunk_x_count || yp < 0 || yp >= chunk_x_count) return nullptr;

			const int index = chunk_indices[xp + yp * chunk_x_count];

			if (index >= 0) return chunks.data() + index;
			else return nullptr;
		}
	}

	void InfiniteTileHandler::dispose() {
		for (int i = 0; i < chunks.size(); ++i) chunks[i].unload();
		if (thread) thread->join();
		if (thread) delete thread;
		thread = nullptr;
		if (loading) {
			loading->tiles = nullptr;
			loading->unload();
			delete loading;
		}
		delete[] chunk_indices;
	}

	void InfiniteTileHandler::queueChunk(int x, int y) {
		loading = new Chunk(x, y, chunk_size, generation, this);
		loading->remesh();

		thread_finished = true;
	}
}

/*int Chunk::generateMesh(int x, int y, graphics::Sprite* output) {
		int neighbours[9];

		neighbours[0] = tile_types[getTile(x - 1, y - 1)].borders_id;
		neighbours[1] = tile_types[getTile(x - 0, y - 1)].borders_id;
		neighbours[2] = tile_types[getTile(x + 1, y - 1)].borders_id;
		neighbours[3] = tile_types[getTile(x - 1, y - 0)].borders_id;
		neighbours[4] = tile_types[getTile(x - 0, y - 0)].borders_id;
		neighbours[5] = tile_types[getTile(x + 1, y - 0)].borders_id;
		neighbours[6] = tile_types[getTile(x - 1, y + 1)].borders_id;
		neighbours[7] = tile_types[getTile(x - 0, y + 1)].borders_id;
		neighbours[8] = tile_types[getTile(x + 1, y + 1)].borders_id;

		const int indices_edges[4] = { 1, 5, 7, 3 };
		const int indices_corners[4] = { 0, 2, 8, 6 };

		char output_priorities[Tile::generation_limit] = {0};

		int types[8] = { 0 };
		int edge_type_count = 0;
		char edges[8] = { 0 };

		const int priority = neighbours[4] >= 0 ? tile_borders[neighbours[4]].priority >> 4 : 0;
		const bool inside = neighbours[4] >= 0 ? tile_borders[neighbours[4]].inside : false;

		for (int i = 0; i < 4; ++i) {
			int id = neighbours[indices_edges[i]];
			if (id == neighbours[4] || id < 0) continue;
			int prio = tile_borders[id].priority >> 4;
			if (prio <= priority || tile_borders[id].inside) continue;
			int type = edge_type_count;
			for (int j = 0; j < edge_type_count; ++j) {
				if (types[j] == id) {
					type = j;
					goto no_new_edge_type;
				}
			}
			types[edge_type_count] = id;
			++edge_type_count;
			no_new_edge_type:
			edges[type] |= 1 << i;
		}

		if (inside) {
			types[edge_type_count] = neighbours[4];
			++edge_type_count;
			for (int i = 0; i < 4; ++i) {
				int id = neighbours[indices_edges[i]];
				if (id == neighbours[4]) continue;
				edges[edge_type_count - 1] |= 1 << i;
			}
		}

		unsigned char corners[4] = { 0 };
		char pre_corners[4] = { 0 };
		int type_count = edge_type_count;

		for (int i = 0; i < 4; ++i) {
			int id = neighbours[indices_corners[i]];
			if (id == neighbours[4] || id < 0) continue;
			int prio = tile_borders[id].priority >> 4;
			if (prio <= priority || tile_borders[id].inside) continue;
			corners[i] = id + 1;
			pre_corners[i] = id;
		}

		if (inside) {
			for (int i = 0; i < 4; ++i) {
				int id = neighbours[indices_corners[i]];
				if (id == neighbours[4]) continue;
				int prio = id >= 0 ? tile_borders[id].priority >> 4 : 0;
				if (prio >= priority) continue;
				corners[i] = neighbours[4] + 1;
				pre_corners[i] = neighbours[4];
			}
		}

		int sprite_index = 1;

		#define DISABLE_CORNER(corner) if (corners[corner] - 1 == types[i]) corners[corner] = 0;

		for (int i = 0; i < edge_type_count; ++i) {
			switch (edges[i]) {
				//single edges
			case 0b0001:
				DISABLE_CORNER(0);
				DISABLE_CORNER(1);
				output[sprite_index] = tile_borders[types[i]].sprites[1]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0010:
				DISABLE_CORNER(1);
				DISABLE_CORNER(2);
				output[sprite_index] = tile_borders[types[i]].sprites[3]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0100:
				DISABLE_CORNER(2);
				DISABLE_CORNER(3);
				output[sprite_index] = tile_borders[types[i]].sprites[2]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1000:
				DISABLE_CORNER(0);
				DISABLE_CORNER(3);
				output[sprite_index] = tile_borders[types[i]].sprites[0]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
				//two adjacent edges
			case 0b1001:
				DISABLE_CORNER(0);
				DISABLE_CORNER(1);
				DISABLE_CORNER(3);
				output[sprite_index] = tile_borders[types[i]].sprites[9]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0011:
				DISABLE_CORNER(0);
				DISABLE_CORNER(1);
				DISABLE_CORNER(2);
				output[sprite_index] = tile_borders[types[i]].sprites[8]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0110:
				DISABLE_CORNER(1);
				DISABLE_CORNER(2);
				DISABLE_CORNER(3);
				output[sprite_index] = tile_borders[types[i]].sprites[10]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1100:
				DISABLE_CORNER(0);
				DISABLE_CORNER(2);
				DISABLE_CORNER(3);
				output[sprite_index] = tile_borders[types[i]].sprites[11]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
				//all but one edge
			case 0b1110:
				for (int j = 0; j < 4; ++j) DISABLE_CORNER(j);
				output[sprite_index] = tile_borders[types[i]].sprites[14]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1101:
				for (int j = 0; j < 4; ++j) DISABLE_CORNER(j);
				output[sprite_index] = tile_borders[types[i]].sprites[15]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1011:
				for (int j = 0; j < 4; ++j) DISABLE_CORNER(j);
				output[sprite_index] = tile_borders[types[i]].sprites[13]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0111:
				for (int j = 0; j < 4; ++j) DISABLE_CORNER(j);
				output[sprite_index] = tile_borders[types[i]].sprites[12]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
				//all edges
			case 0b1111:
				for (int j = 0; j < 4; ++j) DISABLE_CORNER(j);
				output[sprite_index] = tile_borders[types[i]].sprites[16]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
				//two opposite edges
			case 0b1010:
				for (int j = 0; j < 4; ++j) DISABLE_CORNER(j);
				output[sprite_index] = tile_borders[types[i]].sprites[17]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0101:
				for (int j = 0; j < 4; ++j) DISABLE_CORNER(j);
				output[sprite_index] = tile_borders[types[i]].sprites[18]; output_priorities[sprite_index] = tile_borders[types[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
				break;
			}
		}

		int sprite_indices[4] = { 6, 7, 5, 4 };
		for (int i = 0; i < 4; ++i) {
			if ((int)corners[i] - 1 >= 0) {
				output[sprite_index] = tile_borders[pre_corners[i]].sprites[sprite_indices[i]]; output_priorities[sprite_index] = tile_borders[pre_corners[i]].priority; ++sprite_index; if (sprite_index == Tile::generation_limit) return Tile::generation_limit;
			}
		}

		logic::radixSort(output_priorities + 1, (char*)(output + 1), sprite_index - 1, sizeof(graphics::Sprite));

		return sprite_index;
	}*/