#pragma once
#include <vector>
#include "../logic/Matrices.h"

#include "../graphics/FrameBuffer.h"

#include "../graphics/VertexArray.h"

namespace pixelgame {
	struct PointLight {
		glm::vec2 position;
		glm::vec4 color;
		float size = 0.;
		bool casts_shadows = false;

		PointLight() = default;

		PointLight(glm::vec2 position, glm::vec4 color, float size = 0., bool casts_shadows = false);
	};

	struct LightSystem {
		std::vector<fgr::Instance> shadow_sources;
		std::vector<PointLight> lights;

		float shadow_depth;

		fgr::FrameBuffer fbo;
		fgr::VertexArray light_va;
		fgr::InstanceArray shadow_array;

		LightSystem() = default;

		void init();

		void update_shadows();

		void render_lights(glm::mat3& transformations, glm::ivec2 fbo_dimensions);

		void apply_lighting(fgr::FrameBuffer& fbo);
	};
}