#pragma once
#include "../graphics/VertexArray.h"

namespace pixelgame {
	struct Flame {
		glm::vec2 position;
		float y = 0;
		glm::vec2 velocity;
		glm::vec2 size_const;
		float time = 0, time_last = 0, time_delta;
		float random_movement, random_mean, x_last;

		Flame(const glm::vec2& position, glm::vec3 movement_properties, float lifetime, float size);

		void update(const float delta_time);
	};

	struct FireSource {

	};

	struct FireRenderer {
		fgr::VertexArray va;
		std::vector<Flame> flames;
		std::vector<fgr::Vertex> vertices;

		FireRenderer() = default;

		void init();

		void update(const float delta_time);

		void render(const glm::mat3& transform);
	};
}