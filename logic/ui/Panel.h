#pragma once
#include "../../graphics/Sprite.h"
#include "../../graphics/TextRendering.h"

namespace fui {
	struct Panel;

	struct Component {
		Panel* parent_panel;
		glm::ivec2 pos;

		virtual uint getSize() = 0;

		virtual void init(Panel* panel) {};

		virtual void update(float delta_time) {};

		virtual void render() {};

		virtual void dispose() {};

		virtual ~Component();
	};

	struct Panel {
		flo::ScaleMode scalemode;
		glm::mat3 transform_matrix;
		int pixel_size;

		std::vector<Component*> components;
		glm::ivec2 pixel_offset;
		glm::vec2 screen_pos;

		glm::vec2 mouse_pos;

		fgr::BitmapTextRenderer text_renderer;
		fgr::TextureStorage* texture;
		fgr::SpriteArray sprites;

		int initial_update = 0;

		Panel() = default;

		Panel(fgr::TextureStorage& texture, flo::ScaleMode scalemode, glm::ivec2 pixel_offset, glm::vec2 screen_pos);

		void init(std::vector<Component*>& components);

		void update(float delta_time, glm::ivec2 screen_size);

		void render();

		void dispose();
	};
}