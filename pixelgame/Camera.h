#pragma once
#include "../graphics/FrameBuffer.h"
#include "../graphics/VertexArray.h"
#include "../graphics/Sprite.h"

#include "../logic/Matrices.h"

namespace pixelgame {
	
	struct Camera {
		float x = 0, y = 0;
		float xpre, ypre;
		flo::ScaleMode scalemode, intern_scalemode;
		glm::mat3 transform, transform_inverse;
		glm::vec2 inital_pos;
		glm::vec2 offset;
		glm::vec2 velocity = glm::vec2(0.0);
		fgr::FrameBuffer buffer;
		fgr::VertexArray renderer;
		bool sixteen_bit = false;

		Camera() = default;

		Camera(flo::ScaleMode scalemode);

		void init();

		void update(int window_width, int window_height, float delta_time);

		void showRender();
	};

}