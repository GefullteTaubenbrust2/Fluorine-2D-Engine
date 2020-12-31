#pragma once
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../graphics/Sprite.h"

#include "../graphics/Texture.h"

#include <thread>

namespace flo {
	#define TILE_NEEDS_UPDATE(tile) ((tile & 16384) >> 14)

	#ifndef TILE_SPRITE_SIZE
	#define TILE_SPRITE_SIZE 16
	#endif
	
	///<summary>
	/// The type of tile, expressed as a 16-bit number.
	///</summary>
	typedef u16 TileType;

	///<summary>
	/// An integer division where i.e. -1 / 3 = -1 as opposed to the typical -1 / 3 = 0.
	/// Otherwhise it works like a normal integer division.
	///</summary>
	///<param name="a">The dividant.</param>
	///<param name="b">The divisor.</param>
	int divideFixed(const int a, const int b);

	///<summary>
	/// An integer modulo operation where i.e. -1 % 3 = 2 as opposed to the typical -1 % 3 = 0.
	/// Otherwhise it works like a normal integer modulo operation.
	///</summary>
	///<param name="a">The dividant.</param>
	///<param name="b">The divisor.</param>
	int modFixed(const int a, const int b);

	///<summary>
	/// A struct for specifying how tiles are to be rendered.
	/// Note that advanced properties such as collision have to be added externally;
	/// This is best done by indexing an array of these properties with the tile type.
	///</summary>
	struct Tile {
		static const int generation_limit;

		///<summary>
		/// The sprites of all tilestates.
		///</summary>
		fgr::Sprite sprites[15];

		///<summary>
		/// Tiles with higher priorites will overlap those with lower ones.
		///</summary>
		int priority;

		///<summary>
		/// Should the tile attempt to connect to tiles of higher priorities?
		///</summary>
		bool connect_to_higher;

		Tile() = default;
	};

	///<summary>
	/// A struct for storing and creating tilesets.
	///</summary>
	struct TileSet {
		///<summary>
		/// The texture to be used.
		/// WARNING: The pointer must remain constant, though its contents may be altered.
		///</summary>
		fgr::TextureStorage* texture = nullptr;
		
		///<summary>
		/// The tiles contained in the set.
		///</summary>
		std::vector<Tile> tiles;

		TileSet() = default;

		///<summary>
		/// Construct a tileset. NOTE: This constructor also assigns the NULL tile.
		///</summary>
		///<param name="texture">The texture that will be used.</param>
		TileSet(fgr::TextureStorage* texture);

		///<summary>
		/// Add a tile to the set.
		///</summary>
		///<param name="texture">The subtexture in which the desired texture is located.</param>
		///<param name="bounds">The bounds of the desired sprites within the subtexture.</param>
		///<param name="priority">Tiles with higher priorites will overlap those with lower ones.</param>
		///<param name="connects_to_higher">Should the tile attempt to connect to tiles of higher priorities?</param>
		void addTile(fgr::SubTexture texture, const glm::ivec4& bounds, int priority, bool connects_to_higher);

		///<summary>
		/// Add a tile to the set.
		///</summary>
		///<param name="texture">The subtexture in which the desired texture is located.</param>
		///<param name="full_bounds">The bounds of the sprite for a full tile within the subtexture.</param>
		///<param name="bounds">The bounds of the desired sprites for tile edges within the subtexture.</param>
		///<param name="priority">Tiles with higher priorites will overlap those with lower ones.</param>
		///<param name="connects_to_higher">Should the tile attempt to connect to tiles of higher priorities?</param>
		void addTile(fgr::SubTexture texture, const glm::ivec4& full_bounds, const glm::ivec4& bounds, int priority, bool connects_to_higher);
	};

	///<summary>
	/// A struct for handling infinite Tilemaps.
	///</summary>
	struct InfiniteTileHandler;

	///<summary>
	/// A struct for handling limited chunks of tilemaps. It may be used alone or as part of an InfiniteTileHandler.
	///</summary>
	struct Chunk {
		///<summary>
		/// The position of the chunk in the world (a chunk with position [-1;0] would be adjacent to one with position [0;0]).
		/// WARNING: READ-ONLY!
		///</summary>
		int x, y;

		///<summary>
		/// The width and height of the chunk in tiles.
		/// WARNING: READ-ONLY!
		///</summary>
		int size;
		
		///<summary>
		/// How many tiles are within the chunk?
		/// WARNING: READ-ONLY!
		///</summary>
		int tilecount;

		///<summary>
		/// A pointer to the chunk's tiles.
		/// WARNING: READ-ONLY!
		///</summary>
		TileType* tiles = nullptr;

		///<summary>
		/// The SpriteArray used in rendering.
		///</summary>
		fgr::SpriteArray sprites;
		bool remesh_needed, update_needed = false, inited = false;

		///<summary>
		/// If not a nullptr, this is a reference to the InfiniteTileHandler this chunk is part of.
		/// WARNING: READ-ONLY!
		///</summary>
		InfiniteTileHandler* parent;

		///<summary>
		/// The Tileset that the chunk uses.
		/// WARNING: READ-ONLY!
		///</summary>
		Tile* tileset = nullptr;

		///<summary>
		/// Construct a chunk without setting its contents.
		///</summary>
		///<param name="x">The x-position of the chunk in the world (a chunk with position [-1;0] would be adjacent to one with position [0;0]).</param>
		///<param name="y">The y-position of the chunk in the world (a chunk with position [-1;0] would be adjacent to one with position [0;0]).</param>
		///<param name="size">The width and height of the chunk in tiles.</param>
		///<param name="parent">If not a nullptr, this is a reference to the InfiniteTileHandler this chunk is part of.</param>
		Chunk(int x, int y, int size, InfiniteTileHandler* parent = nullptr);

		///<summary>
		/// Construct a chunk using a function.
		///</summary>
		///<param name="x">The x-position of the chunk in the world (a chunk with position [-1;0] would be adjacent to one with position [0;0]).</param>
		///<param name="y">The y-position of the chunk in the world (a chunk with position [-1;0] would be adjacent to one with position [0;0]).</param>
		///<param name="size">The width and height of the chunk in tiles.</param>
		///<param name="generation">A function for generating terrain.</param>
		///<param name="parent">If not a nullptr, this is a reference to the InfiniteTileHandler this chunk is part of.</param>
		Chunk(int x, int y, int size, TileType(*generation)(int x, int y), InfiniteTileHandler* parent = nullptr);

		///<summary>
		/// Find a tile within or outside of the chunk, if possible.
		///</summary>
		///<param name="x">The x-position in tiles relative to the chunk.</param>
		///<param name="y">The y-position in tiles relative to the chunk.</param>
		///<returns>The found tile.</returns>
		TileType getTile(int x, int y);

		///<summary>
		/// Set a tile within the chunk.
		///</summary>
		///<param name="x">The x-position in tiles relative to the chunk.</param>
		///<param name="y">The y-position in tiles relative to the chunk.</param>
		///<param name="tile">The tile type to set.</param>
		void setTile(int x, int y, short tile);

		///<summary>
		/// Unload the chunk from existance.
		///</summary>
		void unload();

		///<summary>
		/// Update the sprites. You will not need to call this.
		///</summary>
		void remesh();

		///<summary>
		/// Generate the sprites for one tile. You will not need to call this.
		///</summary>
		int generateMesh(int x, int y, fgr::Sprite* output);

		///<summary>
		/// Update the chunk when one of its neighbours has been updated. You will not need to call this.
		///</summary>
		void update_neighbour(int relative_x, int relative_y);

		///<summary>
		/// Call this regularly to keep all sprites updated.
		///</summary>
		void update();

		///<summary>
		/// Render the chunk to the framebuffer.
		///</summary>
		///<param name="transformations">The transformations to apply to the sprites.</param>
		void render(const glm::mat3& transformations);

	private:
		void queueRemesh(int x, int y);
	};

	struct InfiniteTileHandler {
	private:
		int chunk_x_count;
		int* chunk_indices = nullptr;
		glm::ivec2 center_offset = glm::ivec2(0);
		TileType(*generation)(int x, int y);
		glm::ivec4 render_bounds;
		fgr::TextureStorage* tilemap_texture;

		std::thread* thread = nullptr;
		bool thread_ready = true;
		glm::ivec2 queued = glm::ivec2(1000, 1000);
		Chunk* loading = nullptr;
		u8 threadstate = false;
		bool save = false;
		std::string save_location;
		std::vector<Chunk> save_queue, save_lock;

	public:
		///<summary>
		/// The chunks within the handler, unordered.
		/// WARNING: READ-ONLY!
		///</summary>
		std::vector<Chunk> chunks;

		///<summary>
		/// The width and height of all chunks.
		/// WARNING: READ-ONLY!
		///</summary>
		int chunk_size;

		///<summary>
		/// The tileset to use for rendering.
		/// WARNING: READ-ONLY!
		///</summary>
		TileSet tileset;

		InfiniteTileHandler() = default;

		///<summary>
		/// Construct a handler.
		///</summary>
		///<param name="tileset">The tileset to use for rendering.</param>
		///<param name="chunk_size">The width and height of all chunks.</param>
		///<param name="generation">How the terrain will be generated.</param>
		///<param name="render_distance">The maximum distance in chunks in which chunks are still handled.</param>
		InfiniteTileHandler(const TileSet& tileset, int chunk_size, TileType(*generation)(int x, int y), int render_distance = 2);

		///<summary>
		/// Set a location for saved chunks to be written into. When out of range, chunks will simply
		/// be deleted otherwhise.
		///</summary>
		///<param name="path">The path to the folder in which all files will be contained.</param>
		void setSaveLocation(const std::string& path);

		///<summary>
		/// Set the maximum distance in chunks in which chunks are still handled.
		///</summary>
		///<param name="chunks">The distance in chunks.</param>
		void setRenderDistance(int chunks);

		///<summary>
		/// Update the tilemap. Call this frequently.
		///</summary>
		///<param name="transformations">The camera transformations.</param>
		///<param name="transformations_inverse">The camera transformations, inversed.</param>
		void update(const glm::mat3& transformations, const glm::mat3& transformations_inverse);

		///<summary>
		/// Get a tile from the tilemap.
		///</summary>
		///<param name="x">The x-position in absolute tile space.</param>
		///<param name="y">The y-position in absolute tile space.</param>
		///<returns>The tile in question.</returns>
		TileType getTile(int x, int y);

		///<summary>
		/// Set a tile in the tilemap.
		///</summary>
		///<param name="x">The x-position in absolute tile space.</param>
		///<param name="y">The y-position in absolute tile space.</param>
		///<param name="tile">The tile type to set.</param>
		///<returns>Was the operation successfull?</returns>
		bool setTile(int x, int y, short tile);

		///<summary>
		/// Render all chunks that should have been visible on the last update.
		///</summary>
		///<param name="transformations">The transformations to apply.</param>
		void render(const glm::mat3& transformations);

		///<summary>
		/// Destroy everything associated with the handler.
		///</summary>
		void dispose();

		///<summary>
		/// Get a chunk from a specified position.
		///</summary>
		///<param name="x">The absolute x-position, in chunks.</param>
		///<param name="y">The absolute y-position, in chunks.</param>
		///<returns>A pointer to the chunk if one is found, a nullptr otherwhise.</returns>
		Chunk* getChunk(int x, int y);
		
	private:
		void moveFocus(int x, int y);

		void queueChunk(int x, int y);

		void saveChunks();

		void saveChunk(const Chunk & c);
	};
}