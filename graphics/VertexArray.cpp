#include "VertexArray.h"

#include "Window.h"

#include "GErrorHandler.h"

namespace fgr {
	Vertex::Vertex(glm::vec3 position, glm::vec2 tex_coords, glm::vec4 color) : 
		position(position), tex_coords(tex_coords), color(color) {

	}

	Instance::Instance(glm::mat3 transformations, glm::mat3 tex_transform, glm::vec4 color) : 
		transformations(transformations), tex_transform(tex_transform), color(color) {

	}

	void VertexArray::init() {
		graphics_check_external();

		if (VBO) return;

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
		glEnableVertexAttribArray(2);

		transform = glm::mat3(1.0);

		graphics_check_error();
	}

	void VertexArray::draw(const Shader& shader, const unsigned int mode) {
		graphics_check_external();

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glUseProgram(shader.shader_program);
		glUniformMatrix3fv(shader.transformations_uniform, 1, false, glm::value_ptr(transform));

		glDrawArrays(mode, 0, vertices_size);

		glBindVertexArray(0);

		graphics_check_error();
	}

	void VertexArray::update() {
		graphics_check_external();

		if (!vertices_size) return;

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		if (dynamic_allocation) {
			if (vertices_size > vertices_allocated) {
				vertices_allocated = (vertices_size & ~1023) + 1024;
				glBufferData(GL_ARRAY_BUFFER, vertices_allocated * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
			}
		}
		else {
			vertices_allocated = vertices_size;
			glBufferData(GL_ARRAY_BUFFER, vertices_allocated * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
		}

		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size * sizeof(Vertex), &vertices[0]);

		glBindVertexArray(0);

		graphics_check_error();
	}

	void VertexArray::resize(const unsigned int vertices_size) {
		Vertex* next = new Vertex[vertices_size];
		int copied = VertexArray::vertices_size;
		if (copied > vertices_size) copied = vertices_size;
		std::copy(vertices, vertices + copied, next);
		if (vertices) delete[] vertices;
		vertices = next;
		VertexArray::vertices_size = vertices_size;

		update();
	}

	void VertexArray::setVertices(Vertex* vertices, const unsigned int size) {
		if (size != vertices_size) {
			if (VertexArray::vertices) delete[] VertexArray::vertices;
			VertexArray::vertices = new Vertex[size];
			vertices_size = size;
		}
		if (vertices_size == 0 || size == 0 || !vertices) return;
		std::copy(vertices, vertices + size, VertexArray::vertices);
		VertexArray::vertices_size = size;

		update();
	}

	void VertexArray::append(Vertex vertex) {
		Vertex* next = new Vertex[vertices_size + 1];
		if (vertices) {
			delete[] vertices;
			std::copy(vertices, vertices + vertices_size, next);
		}
		vertices = next;
		++VertexArray::vertices_size;

		vertices[vertices_size - 1] = vertex;

		update();
	}

	void VertexArray::setTransformations(const glm::mat3& transform) {
		VertexArray::transform = transform;
	}

	void VertexArray::dispose() {
		graphics_check_external();

		if (!(VAO && VBO)) return;
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);

		if (vertices) delete[] vertices;
		vertices = nullptr;
		vertices_size = 0;

		graphics_check_error();
	}

	void InstanceArray::init() {
		graphics_check_external();

		if (VBO) return;

		va.init();

		glGenBuffers(1, &VBO);
		glBindVertexArray(va.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		const int size = sizeof(Instance);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Instance, transformations)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Instance, transformations) + sizeof(glm::vec3)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Instance, transformations) + 2 * sizeof(glm::vec3)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Instance, tex_transform)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Instance, tex_transform) + sizeof(glm::vec3)));
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Instance, tex_transform) + 2 * sizeof(glm::vec3)));
		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Instance, color)));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glVertexAttribDivisor(9, 1);

		glBindVertexArray(0);

		graphics_check_error();
	}

	void InstanceArray::draw(const Shader& shader, const unsigned int mode) {
		graphics_check_external();

		glBindVertexArray(va.VAO);
		glUseProgram(shader.shader_program);
		const glm::mat3 trans = va.transform;
		glUniformMatrix3fv(shader.transformations_uniform, 1, false, glm::value_ptr(va.transform));

		glDrawArraysInstanced(mode, 0, va.vertices_size, glm::min((unsigned int)instances.size(), instances_allocted));

		glBindVertexArray(0);

		graphics_check_error();
	}

	void InstanceArray::update() {
		graphics_check_external();

		if (!instances.size()) return;

		glBindVertexArray(va.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		if (dynamic_allocation) {
			if (instances.size() > instances_allocted) {
				instances_allocted = (instances.size() & ~1023) + 1024;
				glBufferData(GL_ARRAY_BUFFER, instances_allocted * sizeof(Instance), NULL, GL_DYNAMIC_DRAW);
			}
		}
		else {
			instances_allocted = instances.size();
			glBufferData(GL_ARRAY_BUFFER, instances_allocted * sizeof(Instance), NULL, GL_DYNAMIC_DRAW);
		}

		glBufferSubData(GL_ARRAY_BUFFER, 0, instances.size() * sizeof(Instance), instances.data());

		glBindVertexArray(0);

		graphics_check_error();
	}

	void InstanceArray::dispose() {
		graphics_check_external();
		va.dispose();
		if (VBO) glDeleteBuffers(1, &VBO);
		graphics_check_error();
		instances.clear();
	}
}