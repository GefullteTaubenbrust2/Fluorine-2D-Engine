#include "TextRendering.h"

namespace fgr {
	void BitmapFont::loadFromFile(const std::string& path, int char_size) {
		TextureHandle tex;
		tex.loadFromFile(path);
		BitmapFont::char_size = char_size;

		int i = 0;
		for (int yp = 0; yp < tex.height; yp += char_size) {
			for (int xp = 0; xp < tex.width; xp += char_size) {
				int xs = 2;
				for (int x = xp + char_size - 1; x >= xp; --x) {
					for (int y = yp; y < yp + char_size; ++y) {
						if (tex.data[(x + (tex.height - y - 1) * tex.width) * 4 + 3] > 0) {
							xs = x - xp;
							goto stop;
						}
					}
				}
			stop:
				if (i > 255) break;
				char_widths[i] = xs + 2;
				++i;
			}
		}
		for (int i = 0; i < 32; ++i) char_widths[i] = 0;

		texture_size = tex.width;
		tex.createBuffer(GL_REPEAT, GL_NEAREST);
		texture_index = tex.texture;
		delete[] tex.data;
	}

	BitmapText::BitmapText(BitmapFont& font, int space_width, int linebreak_height, TextFormat format, const glm::vec2& pixel_size, BitmapTextRenderer* renderer) :
		font(&font), space_width(space_width), linebreak_height(linebreak_height), format(format), renderer(renderer) {
		destination_coords = glm::vec4(0.0, 0.0, pixel_size.x, pixel_size.y);
		base_sprite = Sprite(0, glm::mat3(1.), glm::vec4(0.), glm::vec4(1.));
	}

	void BitmapText::update_string(const std::string& str) {
		//return;
		BitmapText::str = str;

		linebreaks.resize(0);
		space_count.resize(1);
		space_count[0] = 0;

		int layer = texture.layer;
		int buffer_width = destination_coords.z - destination_coords.x;
		int buffer_height = destination_coords.w - destination_coords.y;
		int last_space = -1;
		float x = 0;
		int line = 0;
		int last_x = 0;
		u8* chars = (u8*)str.data();
		std::vector<int> widths{ 0 };
		std::vector<bool> paragraphs;
		for (int i = 0; i < str.size(); ++i) {
			int w = 0;

			switch (chars[i]) {
			case(' '):
				last_space = i;
				++space_count[line];
				w = space_width;
				last_x = x;
				break;
			case('\n'):
				w = 0;
				paragraphs.push_back(true);
				linebreaks.push_back(i);
				space_count.push_back(0);
				widths[line] = x - (int)space_count[line] * (int)space_width;
				widths.push_back(0);
				++line;
				x = 0;
				last_space = -1;
				if ((line - BitmapText::line) * linebreak_height > buffer_height) goto end;
				continue;
			default:
				w = (int)font->char_widths[chars[i]];
				break;
			}

			if (x + w >= buffer_width - 1) {
				if (last_space < 0) {
					linebreaks.push_back(i);
					widths[line] = x - (int)space_count[line] * (int)space_width;
					x = 0.0;
					++line;
					paragraphs.push_back(false);
					widths.push_back(0);
					space_count.push_back(0);
				}
				else {
					linebreaks.push_back(last_space);
					i = last_space;
					last_space = -1;
					--space_count[line];
					widths[line] = last_x - (int)space_count[line] * (int)space_width;
					last_x = 0;
					x = 0.0;
					++line;
					paragraphs.push_back(false);
					widths.push_back(0);
					space_count.push_back(0);
					continue;
				}
			}

			x += w;
		}

		lines = line + 1;

		end:

		paragraphs.push_back(true);
		linebreaks.push_back(str.size());
		widths[line] = x - ((int)space_count[line]) * (int)space_width;

		glm::vec2 scale = glm::vec2((float)font->char_size / buffer_width, (float)font->char_size / buffer_height);
		glm::mat3 scaled = flo::translate(flo::scale(glm::mat3(1.0), scale * -2.f), glm::vec2(-1.0, 1.0));

		glm::mat3 tex_scaled = flo::scale(glm::mat3(1.0), -glm::vec2((float)font->char_size / font->texture_size));

		x = 0;
		int y = 0;
		line = glm::max(0, BitmapText::line);

		fgr::Shader::textured_instanced.setInt(0, 0);
		//graphics::Shader::textured_instanced.setInt(1, (int)graphics::TextureUnit::dither_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, font->texture_index);

		switch (format) {
		case block:
		case left:
			x = 0;
			break;
		case right:
			x = buffer_width - widths[line] - (int)space_width * (int)space_count[line] - 1;
			break;
		case center:
			x = (buffer_width - widths[line] - (int)space_width * (int)space_count[line] - 1) * 0.5;
			break;
		}

		renderer->letter_array.instances.clear();

		int start = 0;
		if (line > 0) {
			if (line > linebreaks.size()) return;
			start = linebreaks[line - 1] + 1;
		}

		for (int i = start; i < str.size(); ++i) {
			if (linebreaks[line] == i) {
				++line;
				switch (format) {
				case block:
				case left:
					x = 0;
					break;
				case right:
					x = buffer_width - widths[line] - (int)space_width * (int)space_count[line];
					break;
				case center:
					x = (buffer_width - widths[line] - (int)space_width * (int)space_count[line]) * 0.5;
					break;
				}
				y += linebreak_height;

				if (chars[i] == ' ') continue;
			}

			if (y + font->char_size > buffer_height) break;

			if (chars[i] > 32) {
				glm::mat3 trans = flo::translate(scaled, glm::vec2((float)((int)x + font->char_size) / buffer_width * 2., -(float)(y) / buffer_height * 2.));
				const int xp = (((int)chars[i]) % (int)(font->texture_size / font->char_size)) + 1;
				const int yp = (int)font->texture_size / (int)font->char_size - ((int)chars[i]) / (font->texture_size / font->char_size);
				renderer->letter_array.instances.push_back(fgr::Instance(trans, flo::translate(tex_scaled, glm::vec2((float)(xp)* (float)font->char_size / (float)font->texture_size, (float)(yp)* (float)font->char_size / (float)font->texture_size)), glm::vec4(1.0)));
			}

			if (chars[i] != ' ') {
				x += (int)font->char_widths[chars[i]];
			}
			else {
				if (format == block && !paragraphs[line]) {
					x += (float)(buffer_width - widths[line]) / space_count[line];
				}
				else {
					x += space_width;
				}
			}
		}

		renderer->rendered_buffer->bindFrameBuffer(texture);

		renderer->rendered_buffer->clearFramebuffer(texture);

		renderer->letter_array.update();
		renderer->letter_array.draw(fgr::Shader::textured_instanced, GL_TRIANGLE_STRIP);
		glm::vec4 bounds = renderer->rendered_buffer->getTextureCoordinates(destination_coords, false);
		base_sprite.textureOffset = glm::vec2(bounds.x, bounds.y);
		base_sprite.textureScale = glm::vec2(bounds.z - bounds.x, bounds.w - bounds.y);
		base_sprite.texture_layer = layer;

		RenderTarget::bound.unbind();
	}

	Sprite BitmapText::setSprite(const glm::mat3& matrix, const glm::vec4& color) {
		base_sprite.transform = matrix;
		base_sprite.color = color;
		return base_sprite;
	}

	void BitmapText::free(bool destroy) {
		if (index >= 0) renderer->free(index, destroy);
	}

	void BitmapTextRenderer::init() {
		allocated_texture = true;

		rendered_buffer = new TextureStorage(1024, 1024, 4);
		
		rendered_buffer->createBuffer(GL_REPEAT, GL_NEAREST);
		
		letter_array.init();
		letter_array.va.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(0.0, 0.0, 0.5), glm::vec2(0.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 0.0, 0.5), glm::vec2(1.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(0.0, 1.0, 0.5), glm::vec2(0.0, 1.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(1.0, 1.0), glm::vec4(1.0)),
		}.data(), 4);
		letter_array.dynamic_allocation = true;
	}

	void BitmapTextRenderer::init(TextureStorage& texture) {
		allocated_texture = false;

		rendered_buffer = &texture;

		letter_array.init();
		letter_array.va.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(0.0, 0.0, 0.5), glm::vec2(0.0, 0.0), glm::vec4(1.0)),
				fgr::Vertex(glm::vec3(1.0, 0.0, 0.5), glm::vec2(1.0, 0.0), glm::vec4(1.0)),
				fgr::Vertex(glm::vec3(0.0, 1.0, 0.5), glm::vec2(0.0, 1.0), glm::vec4(1.0)),
				fgr::Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(1.0, 1.0), glm::vec4(1.0)),
		}.data(), 4);
		letter_array.dynamic_allocation = true;
	}

	void BitmapTextRenderer::append(BitmapText* text) {
		glm::vec2 tex_scale = glm::vec2(text->destination_coords.z - text->destination_coords.x, text->destination_coords.w - text->destination_coords.y);
		
		SubTexture index = rendered_buffer->addTexture(tex_scale.x, tex_scale.y, nullptr);
		glm::ivec4 bounds = rendered_buffer->getTextureBounds(index);

		text->destination_coords = bounds;
		text->texture = index;
		text->index = text_objects.size();
		text_objects.push_back(text);
	}

	BitmapText* BitmapTextRenderer::getNewest() {
		return text_objects[text_objects.size() - 1];
	}

	void BitmapTextRenderer::free(int index, bool destroy) {
		rendered_buffer->free(text_objects[index]->texture);
		if (destroy) delete text_objects[index];
		text_objects.erase(text_objects.begin() + index);
		for (int i = 0; i < text_objects.size(); ++i) {
			text_objects[i]->index = i;
		}
	}

	void BitmapTextRenderer::dispose() {
		rendered_buffer->dispose();
		letter_array.dispose();
		if (allocated_texture) delete rendered_buffer;
	}
}