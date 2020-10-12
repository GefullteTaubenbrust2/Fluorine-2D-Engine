#pragma once
#include <iostream>
#include "Sprite.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "../logic/SpriteSheet.h"
#include "../logic/Types.h"

namespace fgr {
	enum TextFormat {
		block = 0,
		left = 1,
		right = 2,
		center = 3
	};

	struct BitmapFont {
		int texture_index;
		int texture_size;
		int char_size;
		i8 char_widths[256];

		BitmapFont() = default;

		void loadFromFile(const std::string& path, int char_size);
	};

	struct BitmapTextRenderer;

	struct BitmapText {
		std::string str;
		Sprite* sprite;
		BitmapFont* font;
		//InstanceArray chars;
		int space_width, linebreak_height;
		TextFormat format;
		int layer = 0;
		glm::vec4 destination_coords;
		BitmapTextRenderer* renderer;

		std::vector<unsigned int> linebreaks, space_count;
		int lines;

		BitmapText() = default;

		BitmapText(BitmapFont& font, int space_width, int linebreak_height, TextFormat format, glm::vec2 pixel_size, BitmapTextRenderer* renderer);

		void update_string(std::string str);

		void setRendering(glm::mat3 transform, glm::vec4 color);
	};

	struct BitmapTextRenderer {
		ArrayTexture rendered_buffer;
		SpriteArray sprites;
		InstanceArray letter_array;
		FrameBuffer fbo;
		std::vector<flo::SpriteSheet> spritesheets;
		std::vector<BitmapText> text_objects;
		fgr::VertexArray clear_array;

		BitmapTextRenderer() = default;

		void init();

		void append(BitmapText& text);

		BitmapText& getNewest();

		void free(int index);

		void render(fgr::Shader& shader = fgr::Shader::sprites_instanced);

		void dispose();
	};
}