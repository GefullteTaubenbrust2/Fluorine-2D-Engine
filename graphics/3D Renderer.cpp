#include "3D Renderer.h"

#include "Meshloader.h"

#include "GErrorHandler.h"

#include <fstream>

namespace fgr {
	Vertex3D::Vertex3D(const glm::vec3& position, const glm::vec3& color, const glm::vec3& normal, const glm::vec2& texCoord) : position(position), color(color), normal(normal), tex_coord(texCoord) {

	}

	void View::setPerspective(const float fov, const float screen_width, const float screen_height) {
		projection = glm::perspective(fov, screen_width / screen_height, 0.3f, 300.f);
	}

	void View::setOrientation(const glm::vec3& camera_pos, const glm::vec3& camera_front, const glm::vec3& camera_up) {
		view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
	}

	MaterialInfo::MaterialInfo(Shader& shader, TextureHandle& texture) :
	shader(&shader), texture(&texture) {
	}

	void MaterialInfo::setUniforms() {
		texture->bindToUnit(fgr::TextureUnit::texture0);
		shader->setInt(0, (int)fgr::TextureUnit::texture0);
		shader->setVec3(1, sun_direction);
		shader->setVec3(2, sun_color);
		shader->setVec3(3, ambient_color);
	}

	void Mesh::init(Shader& shader, View& view) {
		graphics_check_external();

		Mesh::shader = &shader;
		Mesh::view = &view;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, color));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, tex_coord));
		glEnableVertexAttribArray(3);

		projection_uni = glGetUniformLocation(shader.shader_program, "projection");
		view_uni = glGetUniformLocation(shader.shader_program, "view");
		model_uni = glGetUniformLocation(shader.shader_program, "model");

		graphics_check_error();
	}

	void Mesh::setMesh(const std::vector<Vertex3D>& vertices, const std::vector<uint>& indices) {
		Mesh::vertices = vertices;
		Mesh::indices = indices;
		update();
	}

	void Mesh::loadFromOBJ(const std::string& path) {
		std::ifstream input;
		input.open(path, std::ios::ate | std::ios::binary);

		if (!input.is_open()) {
			std::cerr << "Could not load object file\n";
			return;
		}

		input.seekg(0, std::ios::end);
		uint size = input.tellg();
		input.seekg(0, std::ios::beg);
		char* data = new char[size];
		input.read(data, size);

		input.close();

		std::string str = std::string(data, size);
		meshloader::loadMesh(*this, str);

		update();
	}

	void Mesh::update() {
		graphics_check_external();

		if (!vertices.size() || !indices.size()) return;

		glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		if (vertices.size() > vertices_allocated) {
			vertices_allocated = (vertices.size() & ~1023) + 1024;
			glBufferData(GL_ARRAY_BUFFER, vertices_allocated * sizeof(Vertex3D), NULL, GL_DYNAMIC_DRAW);
		}
		if (indices.size() > indices_allocated) {
			indices_allocated = (indices.size() & ~1023) + 1024;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_allocated * sizeof(Vertex3D), NULL, GL_DYNAMIC_DRAW);
		}

		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(uint), &indices[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex3D), &vertices[0]);

		glBindVertexArray(0);

		graphics_check_error();
	}

	void Mesh::render() {
		graphics_check_external();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glUseProgram(shader->shader_program);

		if (apply_matrices) {
			glUniformMatrix4fv(projection_uni, 1, GL_FALSE, glm::value_ptr(view->projection));
			glUniformMatrix4fv(view_uni, 1, GL_FALSE, glm::value_ptr(view->view));
			glUniformMatrix4fv(model_uni, 1, GL_FALSE, glm::value_ptr(model));
		}
		else {
			glUniformMatrix4fv(projection_uni, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
			glUniformMatrix4fv(view_uni, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
			glUniformMatrix4fv(model_uni, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
		}

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		glBindVertexArray(0);

		graphics_check_error();
	}

	void Mesh::dispose() {
		graphics_check_external();

		if (!(VAO && EBO && VBO)) return;
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteVertexArrays(1, &VAO);

		graphics_check_error();
	}
}