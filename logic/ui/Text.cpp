#include "Text.h"

namespace fui {
	Text::Text(glm::ivec2 pos, glm::ivec2 size, std::string str) : 
	size(size), str(str) {
		Component::pos = pos;
	}

	void Text::setStyle(fgr::BitmapFont font, fgr::TextFormat format, int space_width, int linebreak_size, glm::vec4 color) {
		text.format = format;
		text.space_width = space_width;
		text.linebreak_height = linebreak_size;
		Text::font = font;
		text_sprite.color = color;
	}

	void Text::addShadow(glm::ivec2 offset, glm::vec4 color) {
		shadow = true;
		shadow_offset = offset;
		shadow_sprite.color = color;
	}

	void Text::setString(std::string str) {
		Text::str = str;
		update_needed = true;
	}

	uint Text::getSize() {
		return sizeof(Text);
	}

	void Text::init(Panel* panel) {
		text = fgr::BitmapText(font, text.space_width, text.linebreak_height, text.format, (glm::vec2)glm::abs(size), &parent_panel->text_renderer);
		parent_panel->text_renderer.append(&text);
	}

	void Text::update(float delta_time) {
		if (update_needed) {
			text.update_string(str);
			glm::vec4 scol = shadow_sprite.color, tcol = text_sprite.color;

			text_sprite = text.base_sprite;
			text_sprite.transform = flo::scale_and_translate(size * glm::ivec2(1., -1.), pos + glm::ivec2(0, size.y));
			text_sprite.color = tcol;

			shadow_sprite = text.base_sprite;
			shadow_sprite.transform = flo::scale_and_translate(size * glm::ivec2(1., -1.), pos + glm::ivec2(0, size.y) + shadow_offset);
			shadow_sprite.color = scol;
		}
		update_needed = false;
	}

	void Text::render() {
		if (shadow) parent_panel->sprites.pushSprite(shadow_sprite, update_needed);
		parent_panel->sprites.pushSprite(text_sprite, update_needed);
	}

	void Text::dispose() {
		text.free(false);
	}
}