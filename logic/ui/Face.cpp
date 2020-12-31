#include "Face.h"

namespace fui {
	FaceStyle::FaceStyle(glm::vec2 top_left, glm::vec2 inner0, glm::vec2 inner1, glm::vec2 bottom_right, fgr::SubTexture id) :
		top_left(top_left), inner0(inner0), inner1(inner1), bottom_right(bottom_right), unit(id) {

	}

	void FaceStyle::apply(Panel& panel) {
		fgr::TextureStorage& ts = *panel.text_renderer.rendered_buffer;
		glm::ivec4 bounds = ts.getTextureBounds(unit);
		scale = glm::vec2(1.) / glm::vec2(ts.width, ts.height);
		offset = glm::vec2(bounds.x, bounds.y) / glm::vec2(ts.width, ts.height);
		layer = unit.layer;
	}

	Face::Face(glm::ivec2 pos, glm::ivec2 size, FaceStyle style) :
	size(size), style(style) {
		Component::pos = pos;
	}

	uint Face::getSize() {
		return sizeof(Face);
	}

	void Face::init(Panel* panel) {
		sprites.resize(9);
		style.apply(*parent_panel);

		resetSprites();
	}

	void Face::update(float delta_time) {

	}

	void Face::resetSprites() {
		FaceStyle& s = style;

		const int xof1 = s.inner0.x - s.top_left.x;
		const int xof2 = s.bottom_right.x - s.inner1.x;
		const int xof3 = size.x - xof1 - xof2;
		const int yof1 = s.inner0.y - s.top_left.y;
		const int yof2 = s.bottom_right.y - s.inner1.y;
		const int yof3 = size.y - yof1 - yof2;

		glm::vec2 t0 = s.top_left * s.scale + s.offset;
		glm::vec2 t1 = s.inner0 * s.scale + s.offset;
		glm::vec2 t2 = s.inner1 * s.scale + s.offset;
		glm::vec2 t3 = s.bottom_right * s.scale + s.offset;

		sprites[0] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof1, yof1), pos),
			glm::vec4(t0.x, t0.y, t1.x, t1.y), glm::vec4(1.));
		sprites[1] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof3, yof1), pos + glm::ivec2(xof1, 0.0)),
			glm::vec4(t1.x, t0.y, t2.x, t1.y), glm::vec4(1.));
		sprites[2] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof2, yof1), pos + glm::ivec2(size.x - xof2, 0.0)),
			glm::vec4(t2.x, t0.y, t3.x, t1.y), glm::vec4(1.));
		sprites[3] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof1, yof3), pos + glm::ivec2(0., yof1)),
			glm::vec4(t0.x, t1.y, t1.x, t2.y), glm::vec4(1.));
		sprites[4] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof3, yof3), pos + glm::ivec2(xof1, yof1)),
			glm::vec4(t1.x, t1.y, t2.x, t2.y), glm::vec4(1.));
		sprites[5] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof2, yof3), pos + glm::ivec2(size.x - xof2, yof1)),
			glm::vec4(t2.x, t1.y, t3.x, t2.y), glm::vec4(1.));
		sprites[6] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof1, yof2), pos + glm::ivec2(0., size.y - yof2)),
			glm::vec4(t0.x, t2.y, t1.x, t3.y), glm::vec4(1.));
		sprites[7] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof3, yof2), pos + glm::ivec2(xof1, size.y - yof2)),
			glm::vec4(t1.x, t2.y, t2.x, t3.y), glm::vec4(1.));
		sprites[8] = fgr::Sprite(s.layer, flo::scale_and_translate(glm::vec2(xof2, yof2), pos + glm::ivec2(size.x - xof2, size.y - yof2)),
			glm::vec4(t2.x, t2.y, t3.x, t3.y), glm::vec4(1.));

		needs_reset = true;
	}

	void Face::render() {
		parent_panel->sprites.pushSprites(sprites, needs_reset);

		needs_reset = false;
	}

	void Face::dispose() {

	}
}