#include "Blur.h"

namespace fgr {
	Shader blur5_shader, blur10_shader, scale_shader;

	BlurBuffer::BlurBuffer(float blur_radius) : 
	blur_radius(blur_radius) {
	}

	void BlurBuffer::init() {
		if (!blur5_shader.loaded) blur5_shader.loadFromFile("src/shaders/textured.vert", "src/shaders/blur5.frag", std::vector<std::string>{
			"texture", "direction"
		});
		if (!blur10_shader.loaded) blur10_shader.loadFromFile("src/shaders/textured.vert", "src/shaders/blur10.frag", std::vector<std::string>{
			"texture", "direction"
		});
		if (!scale_shader.loaded) scale_shader.loadFromFile("src/shaders/textured.vert", "src/shaders/downscale.frag", std::vector<std::string>{
			"texture", "scale_factor", "pixelsize"
		});
		io_fbo.init(10, 10, GL_RGBA16F, GL_CLAMP_TO_BORDER, GL_LINEAR);
		util_fbo.init(10, 10, GL_RGBA16F, GL_CLAMP_TO_BORDER, GL_LINEAR);

		va.init();
		va.setVertices(std::vector<Vertex>{
			Vertex(glm::vec3(-1., -1., 0.5), glm::vec2(0, 0), glm::vec4(1.)),
			Vertex(glm::vec3(1.0, -1., 0.5), glm::vec2(1, 0), glm::vec4(1.)),
			Vertex(glm::vec3(-1., 1.0, 0.5), glm::vec2(0, 1), glm::vec4(1.)),
			Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(1, 1), glm::vec4(1.)),
		}.data(), 4);
	}

	void BlurBuffer::blur(FrameBuffer& fbo) {
		float radius = increased_blur ? 10 : 5;

		glm::ivec2 size = glm::vec2(fbo.tex_width, fbo.tex_height) / (blur_radius / radius);
		
		io_fbo.resize(size.x, size.y);
		util_fbo.resize(size.x, size.y);

		io_fbo.bind();
		fbo.bindContent(fgr::TextureUnit::misc);

		scale_shader.setInt(0, fgr::TextureUnit::misc);
		scale_shader.setVec2(1, glm::vec2(blur_radius / radius));
		scale_shader.setVec2(2, glm::vec2(1. / (float)fbo.tex_width, 1. / (float)fbo.tex_height));
		va.draw(scale_shader, RendeMode::triangle_strip);

		util_fbo.bind();
		io_fbo.bindContent(TextureUnit::misc);

		Shader& blur_shader = increased_blur ? blur10_shader : blur5_shader;

		blur_shader.setInt(0, TextureUnit::misc);
		blur_shader.setVec2(1, glm::vec2(1.f / (float)util_fbo.tex_width, 0.));
		va.draw(blur_shader, RendeMode::triangle_strip);

		io_fbo.bind();
		util_fbo.bindContent(TextureUnit::misc);

		blur_shader.setVec2(1, glm::vec2(0., 1.f / (float)util_fbo.tex_height));
		va.draw(blur_shader, RendeMode::triangle_strip);

		io_fbo.unbind();
	}

	void BlurBuffer::bindContent(fgr::TextureUnit unit) {
		io_fbo.bindContent(unit);
	}
	
	void BlurBuffer::dispose() {
		io_fbo.dispose(true);
		util_fbo.dispose(true);
		va.dispose();
	}
}
