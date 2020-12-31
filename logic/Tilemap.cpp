#include "Tilemap.h"

#include <fstream>
#include <sstream>

#include "../graphics/Window.h"

#include "../logic/Sort.h"

#include <direct.h>

namespace flo {
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
		const char c = ((uint)(a & (1 << 31)) >> 31);
		return (a - (b - 1) * c) / b;
	}

	int modFixed(const int a, const int b) {
		const char c = ((uint)(a & (1 << 31)) >> 31);
		return (b - 1) * c + (a + c) % b;
	}
#endif

	const int Tile::generation_limit = 8;

	TileSet::TileSet(fgr::TextureStorage* texture) :
	texture(texture) {
		tiles.push_back(Tile());
	}

	void TileSet::addTile(fgr::SubTexture subtex, const glm::ivec4& bounds, int priority, bool connects_to_higher) {
		glm::ivec4 pos = texture->getTextureBounds(subtex);
		if (pos.x < 0) return;
		glm::vec4 tex_bounds = texture->getTextureCoordinates(bounds + glm::ivec4(pos.x, pos.y, pos.x, pos.y), false);
		Tile tile;
		for (int i = 0; i < 15; ++i) {
			const float x = tex_bounds.x + (tex_bounds.z - tex_bounds.x) * (float)i / 15.f;
			tile.sprites[i] = fgr::Sprite(subtex.layer, glm::mat3(1.), glm::vec4(x, tex_bounds.y, x + (tex_bounds.z - tex_bounds.x) / 15.f, tex_bounds.w), glm::vec4(1.));
		}
		tile.priority = priority;
		tile.connect_to_higher = connects_to_higher;
		tiles.push_back(tile);
	}

	void TileSet::addTile(fgr::SubTexture subtex, const glm::ivec4& full_bounds, const glm::ivec4& bounds, int priority, bool connects_to_higher) {
		glm::ivec4 pos = texture->getTextureBounds(subtex);
		if (pos.x < 0) return;
		glm::vec4 tex_bounds = texture->getTextureCoordinates(bounds + glm::ivec4(pos.x, pos.y, pos.x, pos.y), false);
		glm::vec4 full_tex_bounds = texture->getTextureCoordinates(full_bounds + glm::ivec4(pos.x, pos.y, pos.x, pos.y), false);
		Tile tile;
		for (int i = 0; i < 14; ++i) {
			const float x = tex_bounds.x + (tex_bounds.z - tex_bounds.x) * (float)i / 14.f;
			tile.sprites[i + 1] = fgr::Sprite(subtex.layer, glm::mat3(1.), glm::vec4(x, tex_bounds.y, x + (tex_bounds.z - tex_bounds.x) / 14.f, tex_bounds.w), glm::vec4(1.));
		}
		tile.sprites[0] = fgr::Sprite(subtex.layer, glm::mat3(1.), full_tex_bounds, glm::vec4(1.));
		tile.priority = priority;
		tile.connect_to_higher = connects_to_higher;
		tiles.push_back(tile);
	}

	Chunk::Chunk(int x, int y, int size, InfiniteTileHandler* parent) : x(x), y(y), size(size), tilecount(size* size), parent(parent) {
		tiles = new TileType[tilecount * 2];
		remesh_needed = true;
		if (parent) tileset = parent->tileset.tiles.data();
		for (int x = 0; x < size; ++x) {
			for (int y = 0; y < size; ++y) {
				tiles[x + y * size + tilecount] = 1 << 15;
			}
		}
	}

	Chunk::Chunk(int x, int y, int size, TileType(*generation)(int x, int y), InfiniteTileHandler* parent) : x(x), y(y), size(size), tilecount(size*size), parent(parent) {
		tiles = new TileType[tilecount * 2];
		for (int x = 0; x < size; ++x) {
			for (int y = 0; y < size; ++y) {
				tiles[x + y * size] = generation(x + Chunk::x * size, y + Chunk::y * size);
				tiles[x + y * size + tilecount] = 1 << 15;
			}
		}
		remesh_needed = true;

		if (parent) tileset = parent->tileset.tiles.data();
	}

	TileType Chunk::getTile(int x, int y) {
		if (x < 0 || x >= size || y < 0 || y >= size) {
			if (!parent) return NULL;
			return parent->getTile(x + Chunk::x * size, y + Chunk::y * size);
		}
		return tiles[x + y * size];
	}
	
	void Chunk::setTile(int x, int y, short tile) {
		if (x < 0 || x >= size || y < 0 || y >= size) return;
		remesh_needed = true;
		tiles[x + y * size] = tile;
		if (!parent) return;
		int xpre = 100, ypre = 100;
		for (int xp = -1; xp <= 1; ++xp) {
			for (int yp = -1; yp <= 1; ++yp) {
				int xa = divideFixed(x + xp, size), ya = divideFixed(y + yp, size);
				if (xa == xpre && ya == ypre) continue;
				xpre = xa;
				ypre = ya;

				if (xa < 0 || xa >= size || ya < 0 || ya >= size) {
					Chunk* c = parent->getChunk(Chunk::x + xa, Chunk::y + ya);
					if (c) c->queueRemesh(modFixed(xa, size), modFixed(ya, size));
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
			if (parent) sprites.texture_array = parent->tileset.texture->id;
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
		TileType neighbours[4];

		neighbours[0] = getTile(x, y);
		neighbours[1] = getTile(x + 1, y);
		neighbours[2] = getTile(x, y + 1);
		neighbours[3] = getTile(x + 1, y + 1);

		char output_priorities[Tile::generation_limit] = { 0 };

		TileType types[4] = { 0 };
		int type_count = 0;
		uchar configurations[4] = { 0 };

		for (int i = 0; i < 4; ++i) {
			const short id = neighbours[i];
			if (id == NULL) continue;
			int prio = tileset[id].priority;

			bool new_type = true;
			for (int j = 0; j < type_count; ++j) {
				if (types[j] == id || tileset[types[j]].priority == prio || !id) {
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
			const int prio = tileset[types[i]].priority;
			const bool connects = tileset[types[i]].connect_to_higher;

			for (int j = 0; j < 4; ++j) {
				const int p = tileset[neighbours[j]].priority;
				if (p == prio || (p > prio && connects)) configurations[i] |= 1 << j;
			}

			const Tile& tile = tileset[types[i]];

			switch (configurations[i]) {
				//single corners
			case 0b0001:
				output[write_index] = tile.sprites[7]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0010:
				output[write_index] = tile.sprites[8]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0100:
				output[write_index] = tile.sprites[5]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1000:
				output[write_index] = tile.sprites[6]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
				//single sides
			case 0b0011:
				output[write_index] = tile.sprites[2]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0101:
				output[write_index] = tile.sprites[1]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1010:
				output[write_index] = tile.sprites[4]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1100:
				output[write_index] = tile.sprites[3]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
				//missing corners
			case 0b1110:
				output[write_index] = tile.sprites[11]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1101:
				output[write_index] = tile.sprites[12]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1011:
				output[write_index] = tile.sprites[9]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b0111:
				output[write_index] = tile.sprites[10]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
				//two corners
			case 0b0110:
				output[write_index] = tile.sprites[13]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			case 0b1001:
				output[write_index] = tile.sprites[14]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
				//full
			case 0b1111:
				output[write_index] = tile.sprites[0]; output_priorities[write_index] = prio;
				++write_index; if (write_index > Tile::generation_limit) return Tile::generation_limit;
				break;
			}
		}

		flo::radixSort(output_priorities, (i8*)(output), write_index, sizeof(fgr::Sprite));

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

				Tile tile = tileset[getTile(x, y)];

				glm::mat3 offset = flo::scale(flo::translate(glm::mat3(1.0), glm::vec2(x + Chunk::x * size, y + Chunk::y * size)), glm::vec2(TILE_SPRITE_SIZE));

				int generated = 0;

				generated = generateMesh(x, y, generate);

				int actual_count = (x + y * size == tilecount - 1) ? (sprites.sprites.size() - index) : ((tiles[x + y * size + tilecount + 1] & 0x7fff) - index + add);

				add += generated - actual_count;

				for (int i = 0; i < generated; ++i) {
					generate[i].transform = offset;
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

	void Chunk::render(const glm::mat3& transform) {
		sprites.setTransformations(transform);
		sprites.draw(fgr::Shader::sprites_instanced);
	}

	enum ThreadOutcomes {
		loaded = 1,
		saved = 2,
	};

	InfiniteTileHandler::InfiniteTileHandler(const TileSet& tileset, int chunk_size, TileType(*generation)(int x, int y), int render_distance) :
	chunk_size(chunk_size), generation(generation), tileset(tileset) {
		setRenderDistance(render_distance);
	}

	void InfiniteTileHandler::setSaveLocation(const std::string& path) {
		save_location = path;
		save = true;
		_mkdir(path.data());
	}

	void InfiniteTileHandler::setRenderDistance(int chunks) {
		chunk_x_count = chunks * 2 + 1;
		if (chunk_indices) delete[] chunk_indices;
		chunk_indices = new int[chunk_x_count* chunk_x_count];

		for (int i = 0; i < chunk_x_count * chunk_x_count; ++i) {
			chunk_indices[i] = -1;
		}
	}

	void InfiniteTileHandler::update(const glm::mat3& transformations, const glm::mat3& transformations_inverse) {
		glm::vec2 min = transformations_inverse * glm::vec3(-1.0, 1.0, 1.0);
		glm::vec2 max = transformations_inverse * glm::vec3(1.0, -1.0, 1.0);

		const int xa = divideFixed(min.x, TILE_SPRITE_SIZE * chunk_size);
		const int ya = divideFixed(min.y, TILE_SPRITE_SIZE * chunk_size);
		const int xb = divideFixed(max.x, TILE_SPRITE_SIZE * chunk_size);
		const int yb = divideFixed(max.y, TILE_SPRITE_SIZE * chunk_size);

		render_bounds = glm::vec4(xa, ya, xb, yb);

		if (xa < center_offset.x - chunk_x_count / 2 + 1 || ya < center_offset.y - chunk_x_count / 2 + 1 || xb > center_offset.x + chunk_x_count / 2 - 1 || yb > center_offset.y + chunk_x_count / 2 - 1) {
#if _DEBUG
			std::cout << "moving focus to " << (xa + xb) / 2 << ' ' << (ya + yb) / 2 << '\n';
#endif
			moveFocus((xa + xb) / 2, (ya + yb) / 2);
		}

		if (threadstate && thread) {
			thread->join();
			delete thread;
			thread = nullptr;

			switch (threadstate) {
			case loaded: {
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
					break;
				}
			case saved: {
					for (int i = 0; i < save_lock.size(); ++i) {
						save_lock[i].unload();
					}
#ifdef _DEBUG
					std::cout << "unloading saved chunks\n";
#endif
					break;
				}
			}

			thread_ready = true;
		}

		for (int i = 0; i < chunks.size(); ++i) {
			chunks[i].update();
		}

		if (!thread && thread_ready) {
			bool brk = false;
			for (int x = render_bounds.x - 1; x <= render_bounds.z + 1; ++x) {
				for (int y = render_bounds.y - 1; y <= render_bounds.a + 1; ++y) {
					Chunk* chunk = getChunk(x, y);
					if (!chunk && !thread) {
#if _DEBUG
						std::cout << "queued generation at " << x << ' ' << y << '\n';
#endif
						queued = glm::ivec2(x, y);
						thread = new std::thread(&InfiniteTileHandler::queueChunk, this, x, y);
						threadstate = 0;
						thread_ready = false;
						brk = true;
						break;
					}
				}
				if (brk) break;
			}
			if (!thread && save && save_queue.size() > 0) {
				save_lock = save_queue;
				save_queue.clear();
				thread = new std::thread(&InfiniteTileHandler::saveChunks, this);
				thread_ready = false;
				threadstate = 0;
#if _DEBUG
				std::cout << "saving chunks\n";
#endif
			}
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
				if (!save) chunk.unload();
				else save_queue.push_back(chunk);

				chunks.erase(chunks.begin() + i);
				--i;
				continue;
			}
		}
#if _DEBUG
		std::cout << "chunk count: " << chunks.size() << '\n';
#endif
	}

	TileType InfiniteTileHandler::getTile(int x, int y) {
		Chunk* chunk = getChunk(divideFixed(x, chunk_size), divideFixed(y, chunk_size));
		if (!chunk) return NULL;
		return chunk->getTile(modFixed(x, chunk_size), modFixed(y, chunk_size));
	}

	bool InfiniteTileHandler::setTile(int x, int y, short tile) {
		Chunk* chunk = getChunk(divideFixed(x, chunk_size), divideFixed(y, chunk_size));
		if (!chunk) return false;
		chunk->setTile(modFixed(x, chunk_size), modFixed(y, chunk_size), tile);
		return true;
	}

	void InfiniteTileHandler::render(const glm::mat3& transformations) {
		for (int x = render_bounds.x; x <= render_bounds.z; ++x) {
			for (int y = render_bounds.y; y <= render_bounds.a; ++y) {
				Chunk* chunk = getChunk(x, y);
				if (chunk) chunk->render(transformations);
			}
		}
	}

	Chunk* InfiniteTileHandler::getChunk(int x, int y) {
		const int xp = x - center_offset.x + chunk_x_count / 2;
		const int yp = y - center_offset.y + chunk_x_count / 2;

		if (xp < 0 || xp >= chunk_x_count || yp < 0 || yp >= chunk_x_count) return nullptr;

		const int index = chunk_indices[xp + yp * chunk_x_count];

		if (index >= 0) return chunks.data() + index;
		else return nullptr;
	}

	void InfiniteTileHandler::dispose() {
		if (thread) thread->join();
		if (thread) delete thread;
		thread = nullptr;
		for (int i = 0; i < chunks.size(); ++i) {
			saveChunk(chunks[i]);
			chunks[i].unload();
		}
		if (loading) {
			loading->tiles = nullptr;
			loading->unload();
			delete loading;
		}
		delete[] chunk_indices;
	}

	void InfiniteTileHandler::queueChunk(int x, int y) {
		if (save) {
			std::ostringstream oss;
			oss << save_location << "/x" << x << "y" << y << ".chunk";
			std::ifstream input_stream;
			input_stream.open(oss.str().data(), std::ifstream::binary);
			if (input_stream.is_open()) {
				std::cout << "loading chunk from hard drive\n";
				loading = new Chunk(x, y, chunk_size, this);
				input_stream.read((char*)loading->tiles, loading->tilecount * 2);
				input_stream.close();
				threadstate = loaded;
				return;
			}
		}

		loading = new Chunk(x, y, chunk_size, generation, this);

		threadstate = loaded;
	}

	void InfiniteTileHandler::saveChunks() {
		for (int i = 0; i < save_lock.size(); ++i) {
			saveChunk(save_lock[i]);
		}

		threadstate = saved;
	}

	void InfiniteTileHandler::saveChunk(const Chunk& c) {
		std::ofstream output_stream;
		std::ostringstream oss;
		oss << save_location << "/x" << c.x << "y" << c.y << ".chunk";
		output_stream.open(oss.str().data(), std::ofstream::binary);
		output_stream.write((char*)c.tiles, c.tilecount * sizeof(TileType));
		output_stream.close();
	}
}