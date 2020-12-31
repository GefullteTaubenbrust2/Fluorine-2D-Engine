#pragma once
#include "Panel.h"

namespace fui {
	struct FaceStyle {
		fgr::SubTexture unit;
		int layer;
		glm::vec2 top_left, inner0, inner1, bottom_right;
		glm::vec2 scale, offset;

		FaceStyle() = default;

		FaceStyle(glm::vec2 top_left, glm::vec2 inner0, glm::vec2 inner1, glm::vec2 bottom_right, fgr::SubTexture unit);

		void apply(Panel& panel);
	};

	struct Face : public Component {
		glm::ivec2 size;
		FaceStyle style;
		std::vector<fgr::Sprite> sprites;
		bool needs_reset = true;

		Face() = default;

		Face(glm::ivec2 pos, glm::ivec2 size, FaceStyle style);

		virtual uint getSize() override;

		virtual void init(Panel* panel) override;

		virtual void update(float delta_time) override;

		void resetSprites();

		virtual void render() override;

		virtual void dispose() override;
	};
}