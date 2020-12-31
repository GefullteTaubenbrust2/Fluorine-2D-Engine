#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../ECS.h"

#include "Generator.h"

namespace fui {
	void init_editor(Generator& gen, Panel& panel);

	void editor_selection(flo::EntityComponentSystem& ecs);

	void edit_object(glm::vec2* pos, glm::vec2* scale, float* angle);

	void update_editor(glm::mat3& transformations);

	void render_editor(glm::mat3& transformations);
}