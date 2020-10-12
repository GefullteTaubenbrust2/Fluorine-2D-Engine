#include <iostream>

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GErrorHandler.h"

#include "FrameBuffer.h"

namespace fgr {
	namespace Texture {
		TextureHandle dither0, dither1;
	}

	void init_default_textures() {
		char d0[16] = {
			 0,  8,  2, 10,
			12,  4, 14,  6,
			 3, 11,  1,  9,
			15,  7, 13,  5,
		};
		char d1[16] = {
			 0,  8,  2, 10,
			 4, 12,  6, 14,
			11,  1,  9,  3,
			15,  5, 13,  7,
		};
		Texture::dither0.data = new u8[64];
		Texture::dither1.data = new u8[64];
		Texture::dither0.width = 4;
		Texture::dither0.height = 4;
		Texture::dither1.width = 4;
		Texture::dither1.height = 4;
		for (int i = 0; i < 16; ++i) {
			Texture::dither0.data[i * 4] = d0[i];
			Texture::dither0.data[i * 4 + 1] = d0[i];
			Texture::dither0.data[i * 4 + 2] = d0[i];
			Texture::dither0.data[i * 4 + 3] = 0;

			Texture::dither1.data[i * 4] = d1[i];
			Texture::dither1.data[i * 4 + 1] = d1[i];
			Texture::dither1.data[i * 4 + 2] = d1[i];
			Texture::dither1.data[i * 4 + 3] = 0;
		}

		Texture::dither0.createBuffer(GL_REPEAT, GL_NEAREST);
		Texture::dither1.createBuffer(GL_REPEAT, GL_NEAREST);
	}

	TextureHandle::TextureHandle(int width, int height, int bytes_per_pixel, u8* data) :
		width(width), height(height), data(data), bytes_per_pixel(bytes_per_pixel) {

	}

	void TextureHandle::loadFromFile(const std::string& path, bool flip_vertically) {
		stbi_set_flip_vertically_on_load(flip_vertically);
		unsigned char* loaded = stbi_load(path.data(), &width, &height, &bytes_per_pixel, 4);
		if (data) delete[] data;
		data = new unsigned char[width * height * bytes_per_pixel];
		std::copy(loaded, loaded + width * height * bytes_per_pixel, data);
		stbi_image_free(loaded);
	}

	void TextureHandle::loadFromID(const int id, int texture_type, int texture_format) {
		graphics_check_external();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
		int size = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, &size);
		bytes_per_pixel = size / 2;

		TextureHandle::texture_format = texture_format;
		TextureHandle::texture_type = texture_type;
		TextureHandle::texture = id;

		if (data) delete[] data;
		data = new unsigned char[width * height * bytes_per_pixel];

		glGetTexImage(GL_TEXTURE_2D, 0, texture_format, GL_UNSIGNED_BYTE, data);

		graphics_check_error();
	}

	void TextureHandle::createBuffer(int wrap, int filter, int texture_type, int texture_format) {
		graphics_check_external();

		TextureHandle::texture_format = texture_format;
		TextureHandle::texture_type = texture_type;

		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, texture_type, width, height, 0, texture_format, GL_UNSIGNED_BYTE, data);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

		graphics_check_error();
	}

	void TextureHandle::syncTexture() {
		graphics_check_external();

		int glwidth = 0, glheight = 0;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &glwidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &glheight);

		if (glwidth == width && glheight == height) {
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, texture_format, GL_UNSIGNED_BYTE, data);
		}
		else {
			int wrap = 0, filter = 0;

			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WRAP_S, &wrap);
			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_MIN_FILTER, &filter);

			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &texture);

			createBuffer(wrap, filter, texture_type, texture_format);
		}

		graphics_check_error();
	}

	void TextureHandle::bindToUnit(const TextureUnit unit) {
		graphics_check_external();
		glActiveTexture(UNIT_ENUM_TO_GL_UNIT(unit));
		glBindTexture(GL_TEXTURE_2D, texture);
		graphics_check_error();
	}

	bool TextureHandle::readRect(int x, int y, int width, int height, int format, u8* data) {
		graphics_check_external();
		bindToUnit(fgr::TextureUnit::misc);
		glReadPixels(x, y, width, height, texture_format, format, data);
		return false;
		graphics_check_error();
	}

	void TextureHandle::setRect(int x, int y, int width, int height, u8* data) {
		graphics_check_external();
		bindToUnit(fgr::TextureUnit::misc);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, texture_format, GL_UNSIGNED_BYTE, data);
		graphics_check_error();
	}

	void TextureHandle::dispose() {
		graphics_check_external();
		glBindTexture(GL_TEXTURE_2D, 0);
		if (texture) glDeleteTextures(1, &texture);
		if (data) delete[] data;
		data = nullptr;
		graphics_check_error();
	}

	int TextureHandle::splitTexture(const TextureHandle& texture, int* output, const int width, const int height) {
		graphics_check_external();

		if (texture.width % width && texture.height % height) return 0;

		const int size = texture.width * texture.height / (width * height);
		glGenTextures(size, (GLuint*)output);

		for (int x = 0; x < texture.width / width; ++x) {
			for (int y = 0; y < texture.height / height; ++y) {
				const int index = x + y * texture.width / width;
				glBindTexture(GL_TEXTURE_2D, output[index]);

				char* data = new char[width * height * 4];

				for (int yp = 0; yp < height; ++yp) {
					const int xa = x * width;
					const int ya = y * height + yp;
					std::copy(texture.data + (xa + (ya) * texture.width) * 4, texture.data + (xa + width + (ya) * texture.width) * 4, data + yp * width * 4);
				}

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

				delete[] data;
			}
		}
		graphics_check_error();

		return size;
	}

	ArrayTexture::ArrayTexture(int width, int height, int layer_count, int bytes_per_pixel) : width(width), height(height), layer_count(layer_count), bytes_per_pixel(bytes_per_pixel) {
		data = new u8[width * height * layer_count * bytes_per_pixel];
	}

	void ArrayTexture::loadFromFile(const std::string& path, int layer, bool flip_vertically) {
		stbi_set_flip_vertically_on_load(flip_vertically);
		int lwidth = 0, lheight = 0, lbytes_per_pixel = 0;
		unsigned char* loaded = stbi_load(path.data(), &lwidth, &lheight, &lbytes_per_pixel, 4);
		if (lwidth == width && lheight == height && lbytes_per_pixel == bytes_per_pixel) {
			std::copy(loaded, loaded + width * height * bytes_per_pixel, data + width * height * bytes_per_pixel * layer);
		}
		stbi_image_free(loaded);
	}

	void ArrayTexture::loadFromID(const int id, int texture_type, int texture_format) {
		graphics_check_external();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &width);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &height);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &layer_count);
		int size = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_BLUE_SIZE, &size);
		bytes_per_pixel = size / 2;

		ArrayTexture::texture_format = texture_format;
		ArrayTexture::texture_type = texture_type;
		ArrayTexture::id = id;

		if (data) delete[] data;
		data = new unsigned char[width * height * bytes_per_pixel * layer_count];

		glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, texture_format, GL_UNSIGNED_BYTE, data);

		destroyFBO();

		graphics_check_error();
	}

	void ArrayTexture::createBuffer(int wrap, int filter, int texture_type, int texture_format) {
		graphics_check_external();

		ArrayTexture::texture_type = texture_type;
		ArrayTexture::texture_format = texture_format;

		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, texture_type, width, height, layer_count, 0, texture_format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, filter);

		destroyFBO();

		graphics_check_error();
	}

	void ArrayTexture::syncTexture() {
		graphics_check_external();

		int glwidth = 0, glheight = 0, gldepth = 0;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WIDTH, &glwidth);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_HEIGHT, &glheight);
		glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_DEPTH, &gldepth);

		if (glwidth == width && glheight == height && gldepth == layer_count) {
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, layer_count, texture_format, GL_UNSIGNED_BYTE, data);
		}
		else {
			int wrap = 0, filter = 0;

			glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_WRAP_S, &wrap);
			glGetTexLevelParameteriv(GL_TEXTURE_2D_ARRAY, 0, GL_TEXTURE_MIN_FILTER, &filter);

			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
			glDeleteTextures(1, &id);

			createBuffer(wrap, filter, texture_type, texture_format);
		}

		destroyFBO();

		graphics_check_error();
	}

	FrameBuffer ArrayTexture::createFBO(int layer) {
		graphics_check_external();

		if (fbo) glDeleteFramebuffers(1, &fbo);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, id, 0, layer);

		glGenRenderbuffers(1, &depth_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		fbo_layer = layer;

		FrameBuffer _fbo;
		_fbo.fbo = fbo;
		_fbo.inited = true;
		_fbo.tex_width = width;
		_fbo.tex_height = height;
		_fbo.texture = id;
		_fbo.rbo = depth_rbo;

		graphics_check_error();

		return _fbo;
	}

	void ArrayTexture::destroyFBO() {
		graphics_check_external();
		if (fbo) glDeleteFramebuffers(1, &fbo);
		if (depth_rbo) glDeleteRenderbuffers(1, &depth_rbo);
		fbo_layer = -1;
		graphics_check_error();
	}

	void ArrayTexture::bindToUnit(const TextureUnit unit) {
		graphics_check_external();
		glActiveTexture(UNIT_ENUM_TO_GL_UNIT(unit));
		glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		graphics_check_error();
	}

	bool ArrayTexture::readRect(int x, int y, int width, int height, int layer, unsigned char* data) {
		if (x < 0 || y < 0 || x + width >= ArrayTexture::width || y + height >= ArrayTexture::height || layer < 0 || layer >= layer_count) return true;
		for (int yp = y; yp < y + height; ++yp) {
			std::copy(
				ArrayTexture::data + (layer * ArrayTexture::width * ArrayTexture::height + x + yp * ArrayTexture::width) * 4,
				ArrayTexture::data + (layer * ArrayTexture::width * ArrayTexture::height + x + width + yp * ArrayTexture::width) * 4,
				data + width * (yp - y) * 4);
		}
		return false;
	}

	void ArrayTexture::setRect(int x, int y, int width, int height, int layer, unsigned char* data) {
		graphics_check_external();
		bindToUnit(fgr::TextureUnit::misc);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, layer, width, height, 1, texture_format, GL_UNSIGNED_BYTE, data);
		graphics_check_error();
	}

	void ArrayTexture::dispose() {
		graphics_check_external();
		destroyFBO();
		glBindTexture(GL_TEXTURE_2D, 0);
		if (id) glDeleteTextures(1, &id);
		if (data) delete[] data;
		data = nullptr;
		graphics_check_error();
	}
}