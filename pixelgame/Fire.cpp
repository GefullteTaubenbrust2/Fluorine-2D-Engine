#include "Fire.h"

#include "../logic/Random.h"

#include "../graphics/Renderstate.h"

namespace pixelgame {
	fgr::Shader fire_shader;

	Flame::Flame(const glm::vec2& position, glm::vec3 movement_properties, float lifetime, float size) : 
		position(position), random_movement(movement_properties.x), time_delta(1.0 / lifetime), random_mean(movement_properties.y), size_const(-0.05 * size, -10. / movement_properties.z * size) {
		x_last = position.x;
		velocity.x = flo::random.gaussian(0.0f, random_movement);
		velocity.y = -movement_properties.z;
	}

	void Flame::update(const float delta_time) {
		time += time_delta * delta_time;

		if (time - time_last > 0.1) {
			time_last = std::floor(time * 10) * 0.1;
			x_last = position.x;
			velocity.x = flo::random.gaussian(random_mean, random_movement);
		}

		position.x += velocity.x * delta_time;
		y += velocity.y * delta_time;
	}

	void FireRenderer::init() {
		va.init();
		va.dynamic_allocation = true;
		if (!fire_shader.loaded) fire_shader.loadFromFile("src/shaders/examples/fire.vert", "src/shaders/basic.frag", "src/shaders/examples/fire.geom");

		//va.setVertices(new graphics::Vertex(glm::vec3(0.0, 0.0, 0.0), glm::vec2(1.0, 0.0), glm::vec4(1.0, 0.0, 0.0, 1.0)), 1);
	}

	void FireRenderer::update(const float delta_time) {
		//if (logic::random.next(0, 1) == 0) flames.push_back(Flame(glm::vec2(logic::random.gaussian(0.0, 20.), logic::random.gaussian(0.0, 20.)), 0.02, logic::random.next(0.5, 1.5), 0.05));
		//for (int i = 0; i < 5; ++i) flames.push_back(Flame(glm::vec2(logic::random.next(-200., 200.), logic::random.next(-100., 100.)), glm::vec3(50., 30., logic::random.next(80, 120)), logic::random.next(0.5, 0.99), 4.));
		for (int i = 0; i < 2; ++i) flames.push_back(Flame(glm::vec2(flo::random.next(-100., 80.), 70.), glm::vec3(3., 3., flo::random.next(5., 10.)), flo::random.next(10., 30.), 2.));

		//std::cout << flames.size() << '\n';

		vertices.resize(flames.size());

		for (int i = 0; i < flames.size(); ++i) {
			Flame& fl = flames[i];
			fl.update(delta_time);
			if (fl.time >= 1.) {
				flames.erase(flames.begin() + i);
				--i;
				continue;
			}
			const float f = fl.time < 0.1 ? fl.time * 10. : 1. - fl.time * fl.time;
			vertices[i] = fgr::Vertex(glm::vec3(fl.position.x, fl.position.y, fl.y), fl.velocity * glm::vec2(3., 1.) * (f) * fl.size_const, glm::vec4(1.0 - fl.time, 1.0 - fl.time, 1.0 - fl.time, 1.0));
		}

		//std::cout << flames.size() << '\n';

		vertices.resize(flames.size());

		va.setVertices(vertices.data(), vertices.size());
	}

	void FireRenderer::render(const glm::mat3& transform) {
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);

		va.setTransformations(transform);
		fgr::setDepthTesting(true);
		va.draw(fire_shader, fgr::RendeMode::points);
		fgr::setDepthTesting(false);
	}
}