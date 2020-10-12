#include "Camera.h"
#include "../graphics/Window.h"
#include "../graphics/Texture.h"
#include "../logic/Matrices.h"

#include "../logic/Input.h"

namespace pixelgame {

	Camera::Camera(flo::ScaleMode scalemode) : 
		scalemode(scalemode), 
		intern_scalemode(flo::ScaleModes::constant_width, flo::ScaleModes::no_rounding, scalemode.base_width, scalemode.base_height, scalemode.base_scale, scalemode.pixel_size) {

	}

	void Camera::init() {
		buffer.init(0, 0, sixteen_bit ? GL_RGBA16F : GL_RGBA, GL_CLAMP_TO_EDGE, GL_NEAREST);
		renderer.init();
		renderer.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(1.0, -1.0, 0.5), glm::vec2(1.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(-1.0, -1.0, 0.5), glm::vec2(0.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(1.0, 1.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(-1.0, 1.0, 0.5), glm::vec2(0.0, 1.0), glm::vec4(1.0)),
		}.data(), 4);
	}

	void Camera::update(int window_width, int window_height, float delta_time) {
		int pixel_size = flo::getPixelSize(window_width, window_height, scalemode);

		const int b_width = window_width / pixel_size + 2;
		const int b_height = window_height / pixel_size + 2;

		glm::vec2 scale = flo::applyWindowScale(b_width, b_height, intern_scalemode);

		buffer.resize(b_width, b_height);

		if (flo::getMouseButton(0) == flo::InputType::hit) {
			inital_pos = flo::getMousePosition();
			xpre = x;
			ypre = y;
		}
		else if (flo::getMouseButton(0) == flo::InputType::held) {
			velocity = glm::vec2((float)((flo::getMousePosition().x - inital_pos.x) / pixel_size) * delta_time * 5, (float)((flo::getMousePosition().y - inital_pos.y) / pixel_size) * delta_time * 5);
		}

		x -= velocity.x;
		y += velocity.y;

		float f = delta_time * 5.;
		velocity -= velocity * (f < 0.5 ? f : 0.5f);

		transform = flo::scale_and_translate(scale * glm::vec2(1.0, -1.0), flo::centerWindowOrigin(b_width, b_height) + glm::vec2((int)x, (int)y) * scale);

		//offset = -glm::vec2((int)(-x * pixel_size) % pixel_size, (int)(-y * pixel_size) % pixel_size) / glm::vec2(window_width, window_height) * (float)pixel_size;

		transform_inverse = glm::inverse(transform);

		glm::vec2 sc = glm::vec2((float)(b_width * pixel_size) / (float)window_width, (float)(b_height * pixel_size) / (float)window_height);
		glm::vec2 centered = -flo::centerWindowOrigin(window_width, window_height);
		/*if (pixel_size > 1) centered = -logic::centerWindowOrigin(window_width, window_height) - logic::centerWindowOrigin(b_width * pixel_size, b_height * pixel_size);
		else centered = -logic::centerWindowOrigin(window_width, window_height);*/


		renderer.transform = flo::scale_and_translate(
			sc,
			glm::vec2((int)(x * pixel_size) % pixel_size, (int)(y * pixel_size) % pixel_size) / glm::vec2((float)window_width * 0.5, (float)window_height * 0.5) + centered
		);

		glm::vec2 bottom_right = glm::inverse(renderer.transform) * glm::vec3(1.0, -1.0, 1.0);
		glm::vec2 top_left = glm::inverse(renderer.transform) * glm::vec3(-1.0, 1.0, 1.0);
	}

	void Camera::showRender() {
		glDisable(GL_BLEND);

		buffer.bindContent(fgr::TextureUnit::misc);
		fgr::Shader::textured.setInt(0, (int)fgr::TextureUnit::misc);
		//graphics::Shader::textured.setInt(1, (int)graphics::TextureUnit::dither_texture);

		renderer.draw(fgr::Shader::textured, fgr::RendeMode::triangle_strip);

		glEnable(GL_BLEND);
	}

}