#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fgr {
	enum Blending {
		none = 0,
		linear = 1,
		additive = 2,
		multiplicative = 3,
	};

	void setBlending(Blending blendmode);

	void setDepthTesting(bool depth_test);
}