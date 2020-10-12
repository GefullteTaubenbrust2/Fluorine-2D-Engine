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

	BitmapText::BitmapText(BitmapFont& font, int space_width, int linebreak_height, TextFormat format, glm::vec2 pixel_size, BitmapTextRenderer* renderer) :
		font(&font), space_width(space_width), linebreak_height(linebreak_height), format(format), renderer(renderer) {
		destination_coords = glm::vec4(0.0, 0.0, pixel_size.x, pixel_size.y);
	}

	void BitmapText::update_string(std::string str) {
		//return;
		BitmapText::str = str;

		linebreaks.resize(0);
		space_count.resize(1);
		space_count[0] = 0;

		int buffer_width = destination_coords.z - destination_coords.x;
		int buffer_height = destination_coords.w - destination_coords.y;
		int tex_size = renderer->rendered_buffer.width;
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
				if (line * linebreak_height > buffer_height) goto end;
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

		glm::vec2 scale = glm::vec2((float)font->char_size / tex_size, (float)font->char_size / tex_size);
		glm::mat3 scaled = flo::translate(flo::scale(glm::mat3(1.0), scale * -2.f), glm::vec2(-1.0, 1.0));

		glm::mat3 tex_scaled = flo::scale(glm::mat3(1.0), -glm::vec2((float)font->char_size / font->texture_size));

		x = 0;
		int y = 0;
		line = 0;

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

		for (int i = 0; i < str.size(); ++i) {
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

			if (chars[i] > 32) {
				glm::mat3 trans = flo::translate(scaled, glm::vec2((float)((int)x + font->char_size + destination_coords.x) / tex_size * 2., -(float)(y + destination_coords.y) / tex_size * 2.));
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
		if (renderer->rendered_buffer.fbo_layer != layer) {
			renderer->fbo = renderer->rendered_buffer.createFBO(layer);
		}
		//renderer->fbo.clear(glm::vec4(1.0, 0.0, 0.0, 0.5));
		renderer->fbo.bind();

		glDisable(GL_BLEND);
		renderer->clear_array.setTransformations(flo::scale_and_translate(glm::vec2(2. * (destination_coords.z - destination_coords.x) / (float)tex_size, 2. * (destination_coords.w - destination_coords.y) / (float)tex_size), glm::vec2(destination_coords.x * 2. / (float)tex_size - 1., (-destination_coords.y + tex_size - (destination_coords.w - destination_coords.y)) * 2. / (float)tex_size - 1.)));
		renderer->clear_array.draw(fgr::Shader::basic, fgr::RendeMode::triangle_strip);
		glEnable(GL_BLEND);

		renderer->letter_array.update();
		renderer->letter_array.draw(fgr::Shader::textured_instanced, GL_TRIANGLE_STRIP);
		sprite->textureOffset = glm::vec2(destination_coords.x, -destination_coords.y + tex_size - (destination_coords.w - destination_coords.y)) / (float)tex_size;
		sprite->textureScale = glm::vec2(destination_coords.z - destination_coords.x, destination_coords.w - destination_coords.y) / (float)tex_size;
		sprite->texture_layer = layer;

		renderer->fbo.unbind();

		renderer->sprites.update();
	}

	void BitmapText::setRendering(glm::mat3 transform, glm::vec4 color) {
		sprite->transform = transform;
		sprite->color = color;
		renderer->sprites.update();
	}

	void BitmapTextRenderer::init() {
		const int layer_count = 5;
		rendered_buffer = ArrayTexture(1024, 1024, layer_count, 4);
		fbo.tex_width = rendered_buffer.width;
		fbo.tex_height = rendered_buffer.height;
		//rendered_buffer.data = { 0 };
		delete[] rendered_buffer.data;
		rendered_buffer.data = nullptr;
		rendered_buffer.createBuffer(GL_REPEAT, GL_NEAREST);
		/*for (int i = 0; i < layer_count; ++i) {
			rendered_buffer.createFBO(i);
			fbo.fbo = rendered_buffer.fbo;
			fbo.clear(glm::vec4(0.0));
		}*/
		//std::fill(rendered_buffer.data, rendered_buffer.data + (1024 * 1024 * 4 * layer_count), 0);
		sprites.init();
		//sprites.sprites.push_back(Sprite(0, flo::scale_and_translate(glm::vec2(2.), glm::vec2(-1.)), glm::vec4(0.0, 0.0, 1.0, 1.0), glm::vec4(1.0)));
		//sprites.sprites.push_back(Sprite(0, glm::mat3(0.0), glm::vec4(1.0), glm::vec4(1.0)));
		sprites.update();
		sprites.texture_array = rendered_buffer.id;
		letter_array.init();
		letter_array.va.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(0.0, 0.0, 0.5), glm::vec2(0.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 0.0, 0.5), glm::vec2(1.0, 0.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(0.0, 1.0, 0.5), glm::vec2(0.0, 1.0), glm::vec4(1.0)),
			fgr::Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(1.0, 1.0), glm::vec4(1.0)),
		}.data(), 4);
		letter_array.dynamic_allocation = true;

		for (int i = 0; i < layer_count; ++i) spritesheets.push_back(flo::SpriteSheet(1024, 1024));

		clear_array.init();
		clear_array.setVertices(std::vector<fgr::Vertex>{
			fgr::Vertex(glm::vec3(0.0, 0.0, 0.5), glm::vec2(0.0, 0.0), glm::vec4(0.0)),
			fgr::Vertex(glm::vec3(1.0, 0.0, 0.5), glm::vec2(1.0, 0.0), glm::vec4(0.0)),
			fgr::Vertex(glm::vec3(0.0, 1.0, 0.5), glm::vec2(0.0, 1.0), glm::vec4(0.0)),
			fgr::Vertex(glm::vec3(1.0, 1.0, 0.5), glm::vec2(1.0, 1.0), glm::vec4(0.0)),
		}.data(), 4);

		sprites.texture_array = rendered_buffer.id;
	}

	void BitmapTextRenderer::append(BitmapText& text) {
		glm::vec2 tex_scale = glm::vec2(text.destination_coords.z - text.destination_coords.x, text.destination_coords.w - text.destination_coords.y);
		/*if (next_alloc.y + tex_scale.y < rendered_buffer.height) {
			if (next_alloc.x + tex_scale.x > x_max) x_max = next_alloc.x + tex_scale.x;
		}
		else {
			next_alloc.y = 0.;
			next_alloc.x = x_max;
			if (next_alloc.x + tex_scale.x > x_max) x_max = next_alloc.x + tex_scale.x;
		}
		if (next_alloc.x + tex_scale.x >= rendered_buffer.width) {
			next_alloc = glm::vec2(0.0);
			++allocation_layer;
			x_max = 0;
		}
		text.destination_coords = glm::vec4(next_alloc.x, next_alloc.y, next_alloc.x + tex_scale.x, next_alloc.y + tex_scale.y);
		text.layer = allocation_layer;*/
		//text.destination_coords = glm::vec4(0.0, 0.0, tex_scale.x, tex_scale.y);
		
		glm::vec2 allocation_pos;
		int layer = 0;
		do {
			allocation_pos = spritesheets[layer].allocate(tex_scale);
			++layer;
		} while (layer < 5 && allocation_pos.x < 0);
		--layer;

		if (layer < 5) {
			sprites.sprites.push_back(Sprite(0, glm::mat3(1.0), glm::vec4(1.0), glm::vec4(1.0)));
			text.destination_coords = glm::vec4(allocation_pos.x, allocation_pos.y, allocation_pos.x + tex_scale.x, allocation_pos.y + tex_scale.y);
			text.layer = layer;
			text_objects.push_back(text);
			for (int i = 0; i < text_objects.size(); ++i) {
				text_objects[i].sprite = sprites.sprites.data() + i;
			}
		}
		sprites.update();
	}

	BitmapText& BitmapTextRenderer::getNewest() {
		return text_objects[text_objects.size() - 1];
	}

	void BitmapTextRenderer::free(int index) {
		spritesheets[text_objects[index].layer].free(text_objects[index].destination_coords);
		text_objects.erase(text_objects.begin() + index);
		sprites.sprites.erase(sprites.sprites.begin() + index);
		for (int i = 0; i < text_objects.size(); ++i) {
			text_objects[i].sprite = sprites.sprites.data() + i;
		}
	}

	void BitmapTextRenderer::render(fgr::Shader& shader) {
		sprites.texture_array = rendered_buffer.id;
		sprites.draw(shader);
	}
	void BitmapTextRenderer::dispose() {
		rendered_buffer.dispose();
		sprites.dispose();
		letter_array.dispose();
		clear_array.dispose();
	}
}