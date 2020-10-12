#include "Lighting.h"
#include "../graphics/Renderstate.h"

namespace pixelgame {
	fgr::Shader shadow_shader;
	fgr::Shader light_shader;
	fgr::Shader mask_shader;

	float depth_step = 1. / 256.;

	PointLight::PointLight(glm::vec2 position, glm::vec4 color, float size, bool casts_shadows) :
	position(position), color(color), size(size), casts_shadows(casts_shadows) {

	}

	void LightSystem::init() {
		if (!shadow_shader.loaded) shadow_shader.loadFromFile("src/shaders/instanced/shadow.vert", "src/shaders/basic.frag", "src/shaders/instanced/shadowgen.geom", std::vector<std::string>{"light_position", "depth", "light_radius"});
		if (!light_shader.loaded) light_shader.loadFromFile("src/shaders/textured.vert", "src/shaders/instanced/light.frag", std::vector<std::string>{"c"});
		if (!mask_shader.loaded) mask_shader.loadFromFile("src/shaders/textured.vert", "src/shaders/mask.frag", std::vector<std::string>{"texture1", "texture2"});

		fbo.init(1, 1, GL_RGBA16F, GL_CLAMP_TO_EDGE, GL_NEAREST);
		light_va.init();
		light_va.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(1.0, -1.0, 1.), glm::vec2(1., -1.), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(-1.0, -1.0, 1.), glm::vec2(-1., -1.), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 1.0, 1.), glm::vec2(1., 1.), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(-1.0, 1.0, 1.), glm::vec2(-1., 1.), glm::vec4(1.0)),
		}.data(), 4);
		shadow_array.init();
		shadow_array.va.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(0.0, 0.0, 0.5), glm::vec2(), glm::vec4(1.)),
				fgr::Vertex(glm::vec3(1.0, 0.0, 0.5), glm::vec2(), glm::vec4(1.)),
				fgr::Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(), glm::vec4(1.)),
				fgr::Vertex(glm::vec3(0.0, 1.0, 0.5), glm::vec2(), glm::vec4(1.)),
		}.data(), 4);
	}

	void LightSystem::update_shadows() {
		shadow_array.instances = shadow_sources;
		shadow_array.update();
	}

	void LightSystem::render_lights(glm::mat3& transformations, glm::ivec2 fbo_dimensions) {
		fbo.resize(fbo_dimensions.x, fbo_dimensions.y);

		fbo.clear(glm::vec4(0.0));
		fbo.bind();

		shadow_array.va.setTransformations(transformations);

		shadow_depth = 1.;
		fgr::setBlending(fgr::Blending::additive);
		fgr::setDepthTesting(true);
		for (int i = 0; i < lights.size(); ++i) {
			PointLight& light = lights[i];

			shadow_depth -= depth_step;

			if (light.casts_shadows) {
				shadow_shader.setVec2(0, glm::vec2(light.position));
				shadow_shader.setFloat(1, shadow_depth);
				shadow_shader.setFloat(2, light.size);
				shadow_array.draw(shadow_shader, fgr::RendeMode::line_loop);
			}

			light_va.setTransformations(flo::setDepth(transformations * flo::scale_and_translate(glm::vec2(lights[i].size), light.position), shadow_depth));
			light_shader.setVec4(0, light.color);
			light_va.draw(light_shader, fgr::RendeMode::triangle_strip);
		}
		fgr::setBlending(fgr::Blending::linear);
		fgr::setDepthTesting(false);

		shadow_array.draw(fgr::Shader::basic_instanced, fgr::RendeMode::triangle_fan);

		fbo.unbind();
	}

	void LightSystem::apply_lighting(fgr::FrameBuffer& fbo2) {
		fbo2.bindContent(fgr::TextureUnit::misc2);
		fbo.bindContent(fgr::TextureUnit::misc);
		light_va.setTransformations(flo::setDepth(flo::scale_and_translate(glm::vec2(2.), glm::vec2(-1.)), 0.5));
		mask_shader.setInt(0, (int)fgr::TextureUnit::misc);
		mask_shader.setInt(1, (int)fgr::TextureUnit::misc2);
		light_va.draw(mask_shader, fgr::RendeMode::triangle_strip);
	}
}