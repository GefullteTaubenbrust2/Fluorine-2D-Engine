#pragma once
#include "Panel.h"

namespace fui {
	struct Text : public Component {
		glm::ivec2 size;
		fgr::BitmapFont font;
		fgr::BitmapText text;
		fgr::Sprite text_sprite, shadow_sprite;
		bool shadow = false;
		glm::ivec2 shadow_offset;
		bool update_needed = true;
		std::string str;

		Text() = default;

		Text(glm::ivec2 pos, glm::ivec2 size, std::string str);

		void setStyle(fgr::BitmapFont font, fgr::TextFormat format, int space_width, int linebreak_size, glm::vec4 color);

		void addShadow(glm::ivec2 offset, glm::vec4 color);

		void setString(std::string str);

		virtual uint getSize() override;

		virtual void init(Panel* panel) override;

		virtual void update(float delta_time) override;

		virtual void render() override;

		virtual void dispose() override;
	};
}