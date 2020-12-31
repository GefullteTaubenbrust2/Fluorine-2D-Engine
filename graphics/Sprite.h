#pragma once
#include "VertexArray.h"
#include "../logic/Matrices.h"

namespace fgr {
    ///<summary>
    ///A struct for creating Sprites, rectanles with a texture (an ArrayTexture).
    ///</summary>
	struct Sprite {
		int texture_layer = 0;
		glm::mat3 transform = glm::mat3(0.0);
        ///<summary> Scale of the section of the texure to be rendered, 1.0 being the full texture's size.</summary>
		glm::vec2 textureScale = glm::vec2(0.0);
        ///<summary> Offset from the corner at [0;0] of the section of the texure to be rendered, 1.0 being the full texture's size.</summary>
		glm::vec2 textureOffset = glm::vec2(0.0);
		glm::vec4 color = glm::vec4(0.0);

		Sprite() = default;

		Sprite(const int texture, const glm::mat3& transform, const glm::vec4& textureBounds, const glm::vec4& color);
	};

    ///<summary>
    ///A struct for drawing an array of sprites in few drawcalls.
    ///</summary>
    ///<param name="vertex_path">The path of the vertex shader source file.</param>
    ///<param name="fragment_path">The path of the fragment shader source file.</param>
    ///<param name="uniforms">The names of all accessible uniform variables.</param>
    ///<returns>The success, false being a success.</returns>
	struct SpriteArray {
		private:
		uint VBO = 0;
		uint instances_allocted = 0;
		uint batch_start = 0;
		bool update_required, push_required;
		VertexArray rectangle;

		public:
        ///<summary>
        ///The array_texture to be used, or more specifically, its ID.
        ///</summary>
		uint texture_array;

		uint texture_type = GL_TEXTURE_2D_ARRAY;
        
        ///<summary>
        ///Should be true when the sprites are often changed, false otherwhise.
        ///</summary>
		bool dynamic_allocation = false;
        
        ///<summary>
        ///The array of sprites. This may be changed freely SO LONG AS THE OBJECT IS SUBSEQUENTLY UPDATED!
        ///</summary>
		std::vector<Sprite> sprites;

		SpriteArray() = default;

        ///<summary>
        ///Must be called prior to rendering.
        ///</summary>
		void init();

		///<summary>Start a new batch.</summary>
		void resetBatch();

		///<summary>
		///Push a sprite to the current batch.
		///When this is not enforced and an update is not requested, this will render the sprite in question as in the previous batch.
		///</summary>
		void pushSprite(const Sprite& sprite, bool enforced);

		///<summary>
		///Push multiple sprites to the current batch.
		///When this is not enforced and an update is not requested, this will render the sprites in question as in the previous batch.
		///</summary>
		void pushSprites(const std::vector<Sprite>& sprites, bool enforced);

		///<summary>
		///This will enforce all upcoming sprites pushed in the batch to be updated.
		///</summary>
		void requestUpdate();
        
        ///<summary>
        ///Set the sprite array. Calling "update()" afterwards is not neccessary.
        ///</summary>
        ///<param name="sprites">A vector containing all Sprites.</param>
		void setSprites(const std::vector<Sprite>& sprites);
        
        ///<summary>
        ///Is to be called when changes have been made to the sprites.
        ///</summary>
		void update();

        ///<summary>
        ///Draw the array.
        ///</summary>
        ///<param name="shader">The shader to be used.</param>
		void draw(Shader& shader, int count = -1);

		///<summary>Draw the batch.</summary>
		void drawBatch(Shader& shader);

        ///<summary>
        ///Set a transformation matrix for the entire array.
        ///</summary>
        ///<param name="transform">The transformation matrix.</param>
		void setTransformations(const glm::mat3& transform);
        
        ///<summary>
        ///Destroy all internal data used for rendering.
        ///</summary>
		void dispose();
	};

	struct SpriteBatcher {
		SpriteArray sa;
		std::vector<Sprite>* sprites;
		Shader* shader = &fgr::Shader::sprites_instanced;
		long long id = 0;

		SpriteBatcher() = default;

		SpriteBatcher(Shader& shader);

		void init();

		void pushSprite(const Sprite& sprite);

		void pushSprites(const std::vector<Sprite>& sprites);

		void dispose();
	};

	void finalizeBatch();
}
