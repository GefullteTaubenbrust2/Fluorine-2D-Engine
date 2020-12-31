#include "View.h"

#include "Input.h"

namespace flo {
	View::View(ScaleModes scale_method, ScaleModes rounding, int base_size, fgr::RenderTarget* target) :
	scalemode(scale_method, rounding, base_size, 2. / (float)base_size), target(target) {
	}

	void View::updateViewMatrix() {
		glm::ivec2 target_size = glm::ivec2(target->bounds.z - target->bounds.x, target->bounds.w - target->bounds.y);
		pixelsize = getPixelSize(target_size.x, target_size.y, scalemode);
		glm::vec2 scale = applyWindowScale(target_size.x, target_size.y, scalemode) * glm::vec2(1., -1.);
		transform = scale_and_translate(scale, centerWindowOrigin(target_size.x, target_size.y) + glm::floor(position * (float)pixelsize) / (float)pixelsize * scale);
		transform_inverse = glm::inverse(transform);
	}

	void View::cursorDrag() {
		glm::ivec2 target_size = glm::ivec2(target->bounds.z - target->bounds.x, target->bounds.w - target->bounds.y);

		if (getMouseButton(0) & InputType::hit) {
			initial_mouse_pos = getMousePosition();
			initial_pos = position;
		}
		if (getMouseButton(0) & InputType::held) {
			position = initial_pos + (getMousePosition() - initial_mouse_pos) / (float)pixelsize;
		}
	}
}
