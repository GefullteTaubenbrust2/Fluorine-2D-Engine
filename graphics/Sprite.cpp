#include "Sprite.h"

#include "Texture.h"

#include "GErrorHandler.h"

namespace fgr {
	Sprite::Sprite(const int texture, const glm::mat3& transform, const glm::vec4& textureBounds, const glm::vec4& color) :
		texture_layer(texture),
		transform(transform),
		textureScale(textureBounds.z - textureBounds.x, textureBounds.a - textureBounds.y),
		textureOffset(textureBounds.x, textureBounds.y),
		color(color) {

	}

	void SpriteArray::init() {
		graphics_check_external();

		if (VBO) return;

		rectangle.init();
		rectangle.setVertices(std::vector<Vertex>{
			fgr::Vertex(glm::vec3(0.0, 0.0, 0.5), glm::vec2(0.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 0.0, 0.5), glm::vec2(1.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(0.0, 1.0, 0.5), glm::vec2(0.0, 1.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(1.0, 1.0), glm::vec4(1.0)),
		}.data(), 4);

		glGenBuffers(1, &VBO);
		glBindVertexArray(rectangle.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		const int size = sizeof(Sprite);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Sprite, transform)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Sprite, transform) + sizeof(glm::vec3)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Sprite, transform) + 2 * sizeof(glm::vec3)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Sprite, textureScale)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Sprite, textureOffset)));
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, size, (void*)(offsetof(Sprite, color)));
		glEnableVertexAttribArray(9);
		glVertexAttribPointer(9, 1, GL_INT, GL_FALSE, size, (void*)(offsetof(Sprite, texture_layer)));

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

	void SpriteArray::setSprites(const std::vector<Sprite>& set) {
		graphics_check_external();

		sprites.resize(set.size());
		std::copy(set.data(), set.data() + set.size(), sprites.data());
		update();

		graphics_check_error();
	}

	void SpriteArray::draw(Shader& shader, int count) {
		graphics_check_external();

		if (count < 0) count = sprites.size();

		glActiveTexture(UNIT_ENUM_TO_GL_UNIT(fgr::TextureUnit::misc));
		glBindTexture(texture_type, texture_array);
		
		glBindVertexArray(rectangle.VAO);
		glUseProgram(shader.shader_program);
		const glm::mat3 trans = rectangle.transform;
		glUniformMatrix3fv(shader.transformations_uniform, 1, false, glm::value_ptr(rectangle.transform));
		shader.setInt(0, (int)fgr::TextureUnit::misc);
		
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, rectangle.vertices_size, glm::min((unsigned int)count, instances_allocted));

		glBindVertexArray(0);

		graphics_check_error();
	}

	void SpriteArray::update() {
		if (!sprites.size()) return;

		graphics_check_external();

		glBindVertexArray(rectangle.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		if (dynamic_allocation) {
			int instances_alloc = (sprites.size() & ~1023) + 1024;
			if (instances_alloc > instances_allocted) {
				instances_allocted = instances_alloc;
				glBufferData(GL_ARRAY_BUFFER, instances_allocted * sizeof(Sprite), NULL, GL_DYNAMIC_DRAW);
			}
		}
		else {
			instances_allocted = sprites.size();
			glBufferData(GL_ARRAY_BUFFER, instances_allocted * sizeof(Sprite), NULL, GL_DYNAMIC_DRAW);
		}

		glBufferSubData(GL_ARRAY_BUFFER, 0, sprites.size() * sizeof(Sprite), sprites.data());

		glBindVertexArray(0);

		graphics_check_error();
	}

	void SpriteArray::setTransformations(const glm::mat3& transform) {
		rectangle.setTransformations(transform);
	}

	void SpriteArray::dispose() {
		graphics_check_external();
		rectangle.dispose();
		if (VBO) glDeleteBuffers(1, &VBO);
		graphics_check_error();
		sprites.clear();
	}

	SpriteBatcher* current_batch = nullptr;
	long long current_batch_id = 0;
	int current_sprite_index = 0;

	SpriteBatcher::SpriteBatcher(Shader& shader) : shader(&shader) {

	}

	void SpriteBatcher::init() {
		sa.init();
		sa.dynamic_allocation = true;
		sprites = &sa.sprites;
		id = ++current_batch_id;
	}

	void SpriteBatcher::pushSprite(Sprite& sprite) {
		long long _id = 0;
		if (current_batch) {
			_id = current_batch->id;
		}
		if (_id != id) {
			finalizeBatch();
			current_batch = this;
		}
		if (sprites->size() <= current_sprite_index) sprites->resize(current_sprite_index + 16);
		(*sprites)[current_sprite_index] = sprite;
		++current_sprite_index;
	}

	void SpriteBatcher::dispose() {
		sa.dispose();
	}

	void finalizeBatch() {
		if (!current_batch) return;

		current_batch->sa.update();
		current_batch->sa.draw(*current_batch->shader, current_sprite_index);

		current_batch->sprites->clear();

		current_sprite_index = 0;
	}
}