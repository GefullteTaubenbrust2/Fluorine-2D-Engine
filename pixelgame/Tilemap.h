#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../graphics/Sprite.h"

#include "../graphics/Texture.h"

#include <thread>

namespace pixelgame {
	#define TILE_NEEDS_UPDATE(tile) ((tile & 16384) >> 14)

	#define TILE_SPRITE_SIZE 16

	int divideFixed(const int a, const int b);

	int modFixed(const int a, const int b);

	struct Tile {
		static const int generation_limit;

		fgr::Sprite sprites[13];

		int priority;
		bool connect_to_higher;
		bool collision = false;

		Tile() = default;

		Tile(glm::vec4 bounds, int layer, int priority, bool connect_to_higher);

		Tile(glm::vec4 bounds_full, glm::vec4 bounds_partial, int layer, int priority, bool connect_to_higher);

		Tile(glm::vec4 bounds_full, glm::vec4 bounds_partial, int layer, int priority, bool connect_to_higher, bool collision);
	};

	/*struct SeamlessTile {
		int priority;
		graphics::Sprite sprites[19];
		bool inside;

		SeamlessTile() = default;

		SeamlessTile(glm::vec4 bounds, int layer, int priority, bool inside = false);
	};*/

	extern fgr::ArrayTexture tilemap;
	extern const Tile tile_types[20];
	//extern const SeamlessTile tile_borders[10];

	enum TileName {
		null = 0,
		grass = 1,
		grass1 = 2,
		grass2 = 3,
		grass3 = 4,
		grass_elevated = 5,
		grass1_elevated = 6,
		grass2_elevated = 7,
		grass3_elevated = 8,
		sand = 9,
		sand1 = 10,
		sand2 = 11,
		sand3 = 12,
		water = 13,
		smooth_stone = 14,
		black = 1,
		blocked = 2,
		black2 = 3,
		black3 = 4,
		deflector_straight = 5,
		deflector_diag_0 = 6,
		deflector_diag_1 = 7,
		deflector_diag_2 = 8,
		deflector_diag_3 = 9,
		bg_tile_standard = 10,
		bg_tile_rombus0 = 11,
		bg_tile_rombus0s = 12,
		bg_tile_weld = 13,
		goal = 14,
		abyss = 15,
		bg_tile_rombus1 = 16,
	};

	void initTileset();

	struct InfiniteTileHandler;

	struct Chunk {
		int x, y;
		int size, tilecount;
		unsigned short* tiles = nullptr;
		fgr::SpriteArray sprites;
		bool remesh_needed, update_needed = false, inited = false;
		InfiniteTileHandler* parent;

		Chunk(int x, int y, int size, short(*generation)(int x, int y), InfiniteTileHandler* parent);

		short getTile(int x, int y);

		void setTile(int x, int y, short tile);

		void unload();

		void remesh();

		int generateMesh(int x, int y, fgr::Sprite* output);

		void update_neighbour(int relative_x, int relative_y);

		void update();

		void render(glm::mat3 transformations);

	private:
		void queueRemesh(int x, int y);
	};
	/*
	struct FiniteTileHandler {
		int width, height;
		short* tiles;

		short getTile(int x, int y);

		void setTile(int x, int y, short tile);

		void setTiles(int xmin, int ymin, int width, int height, short* tiles);
	};*/

	struct InfiniteTileHandler {
	private:
		int chunk_x_count;
		int* chunk_indices = nullptr;
		glm::ivec2 center_offset = glm::ivec2(0);
		short(*generation)(int x, int y);
		glm::ivec4 render_bounds;

		std::thread* thread = nullptr;
		glm::ivec2 queued = glm::ivec2(1000, 1000);
		Chunk* loading = nullptr;
		bool thread_finished = false;

	public:
		std::vector<Chunk> chunks;
		int chunk_size;

		InfiniteTileHandler() = default;

		InfiniteTileHandler(int chunk_size, short(*generation)(int x, int y), int render_distance = 2);

		void setRenderDistance(int chunks);

		void update(glm::mat3& transformations, glm::mat3& transformations_inverse);

		short getTile(int x, int y, bool force = false);

		bool setTile(int x, int y, short tile, bool force = false);

		void render(glm::mat3& transformations);

		void dispose();

		Chunk* getChunk(int x, int y, bool force = false);
		
	private:
		Chunk* createChunk(int x, int y);

		void moveFocus(int x, int y);

		void queueChunk(int x, int y);
	};
}