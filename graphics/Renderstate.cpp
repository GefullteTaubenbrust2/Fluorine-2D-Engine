#include "Renderstate.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "GErrorHandler.h"

#include "Sprite.h"

namespace fgr {

	void setBlending(Blending blendmode) {
		graphics_check_external();
		switch (blendmode) {
		case Blending::none:
			glDisable(GL_BLEND);
			break;
		case Blending::linear:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case Blending::additive:
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			break;
		case Blending::multiplicative:
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;
		}
		graphics_check_error();
	}

	void setDepthTesting(bool depth_test) {
		if (depth_test) {
			glDepthFunc(GL_LESS);
			glEnable(GL_DEPTH_TEST);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}
}