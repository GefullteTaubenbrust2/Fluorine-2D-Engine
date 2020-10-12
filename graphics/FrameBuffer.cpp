#include "FrameBuffer.h"

#include "Window.h"

#include <iostream>

#include "GErrorHandler.h"

namespace fgr {
	unsigned int FrameBuffer::bound = 0;

	void FrameBuffer::init(const int width, const int height, const unsigned int fmt, const unsigned int wrap, const unsigned int filter) {
		graphics_check_external();

		tex_width = width;
		tex_height = height;
		if (inited) dispose(true);
		inited = true;

		FrameBuffer::format = fmt;
		FrameBuffer::filter = filter;
		FrameBuffer::wrap = wrap;

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		graphics_check_error();
	}

	void FrameBuffer::resize(const int width, const int height) {
		graphics_check_external();

		if (!inited) {
			return;
		}

		if (width != tex_width || height != tex_height) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glDeleteTextures(1, &texture);

			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

			glDeleteRenderbuffers(1, &rbo);

			glGenRenderbuffers(1, &rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		tex_width = width;
		tex_height = height;

		graphics_check_error();
	}

	void FrameBuffer::bindContent(const TextureUnit unit) {
		graphics_check_external();

		glActiveTexture(UNIT_ENUM_TO_GL_UNIT(unit));
		glBindTexture(GL_TEXTURE_2D, texture);

		graphics_check_error();
	}

	void FrameBuffer::bind() {
		graphics_check_external();

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, tex_width, tex_height);
		FrameBuffer::bound = fbo;

		graphics_check_error();
	}

	void FrameBuffer::unbind() {
		graphics_check_external();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window::width, window::height);
		FrameBuffer::bound = 0;

		graphics_check_error();
	}

	void FrameBuffer::dispose(bool clearTexture) {
		graphics_check_external();

		if (clearTexture && texture) glDeleteTextures(1, &texture);
		if (fbo) glDeleteFramebuffers(1, &fbo);
		if (rbo) glDeleteRenderbuffers(1, &rbo);

		graphics_check_error();
	}

	void FrameBuffer::clear(const glm::vec4& color) {
		graphics_check_external();

		bind();
		glClearColor(color.r, color.g, color.b, color.a);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		unbind();

		graphics_check_error();
	}

	void FrameBufferMS::init(const int width, const int height, const unsigned int format) {
		graphics_check_external();

		tex_width = width;
		tex_height = height;
		if (inited) dispose();
		inited = true;

		FrameBufferMS::format = format;

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, format, width, height, GL_TRUE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

		glGenTextures(1, &depth_tex);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_tex);

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT, width, height, GL_TRUE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_tex, 0);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		graphics_check_error();
	}

	void FrameBufferMS::resolve(const unsigned int output_fbo) {
		graphics_check_external();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output_fbo);
		glBlitFramebuffer(0, 0, tex_width, tex_height, 0, 0, tex_width, tex_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		//glBlitFramebuffer(0, 0, tex_width, tex_height, 0, 0, tex_width, tex_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		graphics_check_error();
	}

	void FrameBufferMS::resize(const int width, const int height) {
		graphics_check_external();

		if (!inited) {
			return;
		}

		if (width != tex_width || height != tex_height) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			glDeleteTextures(1, &texture);

			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);

			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, format, width, height, GL_TRUE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

			glDeleteTextures(1, &depth_tex);

			glGenTextures(1, &depth_tex);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_tex);

			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT, width, height, GL_TRUE);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_tex, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		tex_width = width;
		tex_height = height;

		graphics_check_error();
	}

	void FrameBufferMS::bind() {
		graphics_check_external();

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, tex_width, tex_height);
		FrameBuffer::bound = fbo;

		graphics_check_error();
	}

	void FrameBufferMS::unbind() {
		graphics_check_external();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, window::width, window::height);
		FrameBuffer::bound = 0;

		graphics_check_error();
	}

	void FrameBufferMS::dispose() {
		graphics_check_external();

		if (texture) glDeleteTextures(1, &texture);
		if (fbo) glDeleteFramebuffers(1, &fbo);
		if (depth_tex) glDeleteTextures(1, &depth_tex);

		graphics_check_error();
	}

	void FrameBufferMS::clear(const glm::vec4& color) {
		graphics_check_external();

		bind();
		glClearColor(color.r, color.g, color.b, color.a);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		unbind();

		graphics_check_error();
	}
}