#pragma once
#include <iostream>
#include "Sprite.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "../logic/SpriteSheet.h"
#include "../logic/Types.h"

namespace fgr {
	///<summary>
	/// The format for text e.g. left or right bound.
	///</summary>
	enum TextFormat {
		block = 0,
		left = 1,
		right = 2,
		center = 3
	};

	///<summary>
	/// A struct for creating classic bitmap fonts.
	///</summary>
	struct BitmapFont {
		int texture_index;
		int texture_size;
		int char_size;
		i8 char_widths[256];

		BitmapFont() = default;

		///<summary>
		/// Load the font from an image file.
		///</summary>
		///<param name="path">The path of the file.</param>
		///<param name="char_size">The contained chars must be squares. This parameter is the sidelength of those squares.</param>
		void loadFromFile(const std::string& path, int char_size);
	};

	struct BitmapTextRenderer;

	///<summary>
	/// A struct for handling individual bits of text rendered from bitmap.
	///</summary>
	struct BitmapText {
		std::string str;
		BitmapFont* font;
		int space_width = 2, linebreak_height = 0;
		TextFormat format = TextFormat::left;
		glm::vec4 destination_coords;
		BitmapTextRenderer* renderer;
		fgr::Sprite base_sprite;
		int index = -1;
		int line = 0;
		SubTexture texture;

		std::vector<uint> linebreaks, space_count;
		int lines;

		BitmapText() = default;

		///<summary>
		/// Construct a text object.
		///</summary>
		///<param name="font">The font to use.</param>
		///<param name="space_width">The space width in pixels.</param>
		///<param name="linebreak_height">The linebreak height in pixels, 0 meaning all lines overlap.</param>
		///<param name="format">The text format to use.</param>
		///<param name="pixel_size">The size of the text in pixels.</param>
		///<param name="renderer">The renderer to render to. Note that the text has to be appended to it and it only.</param>
		BitmapText(BitmapFont& font, int space_width, int linebreak_height, TextFormat format, const glm::vec2& pixel_size, BitmapTextRenderer* renderer);

		///<summary>
		/// Set the text to show.
		///</summary>
		///<param name="str">The text to set.</param>
		void update_string(const std::string& str);

		///<summary>
		/// Construct a sprite to draw the text.
		///</summary>
		///<param name="matrix">The transform matrix for the sprite.</param>
		///<param name="color">The sprite's color.</param>
		///<returns>The created sprite.</returns>
		Sprite setSprite(const glm::mat3& matrix, const glm::vec4& color);

		///<summary>
		/// Free the text from its renderer. When not bound to a renderer, nothing will occur.
		///</summary>
		///<param name="destroy">When set to true, the text will attempt to delete itself. This of course only makes sense when it is a pointer.</param>
		void free(bool destroy);
	};

	///<summary>
	/// A struct for centrally storing and handling text.
	///</summary>
	struct BitmapTextRenderer {
		TextureStorage* rendered_buffer;
		InstanceArray letter_array;
		std::vector<BitmapText*> text_objects;
		bool allocated_texture = false;

		BitmapTextRenderer() = default;

		///<summary>
		/// Initialize all OpenGL buffers and objects for use.
		///</summary>
		void init();

		///<summary>
		/// Initialize all OpenGL buffers and objects for use and provide a pre-existing texture to render to.
		///</summary>
		///<param name="texture">A pre-existing texture to render to.</param>
		void init(TextureStorage& texture);

		///<summary>
		/// Bind text to the renderer.
		///</summary>
		///<param name="text">A pointer to the text to bind.</param>
		void append(BitmapText* text);

		///<summary>
		/// Get the text last bound.
		///</summary>
		BitmapText* getNewest();

		///<summary>
		/// Free a text object from the renderer. Alternatively call free() on the text object in question.
		///</summary>
		///<param name="index">The index at which the text object may be found.</param>
		///<param name="destroy">When set to true, the text will attempt to delete itself. This of course only makes sense when it is a pointer.</param>
		void free(int index, bool destroy = false);

		///<summary>
		/// Destroy all allocated contents.
		///</summary>
		void dispose();
	};
}