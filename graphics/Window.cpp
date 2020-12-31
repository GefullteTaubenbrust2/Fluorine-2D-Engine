#include "Window.h"

#include <iostream>

#include "../logic/Time.h"

#include "../logic/Input.h"

#include "Shader.h"

#include "Texture.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include "GErrorHandler.h"

#include "FrameBuffer.h"

#include "Texture.h"

namespace flo {
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void update_input(GLFWwindow* window);

	namespace editor {
		void character_callback(GLFWwindow* window, uint codepoint);
	}

	namespace editor {
		void init_textinput();
	}
}

#undef max

namespace fgr {
	void init_shader_defaults();

	void init_default_textures();

	namespace window {
		int width, height;
		RenderTarget framebuffer;

		glm::ivec2 previous_dimensions;
		int default_width, default_height;

		uint framerate = 10;
		uint frames = 0;
		flo::Stopclock timer;
		float max_tolerance = 0.03;
		GLFWwindow* glfw_window;
		float delta_time, last = 0.0;

		bool window_created = false;

		void(*size_callback)(GLFWwindow* window, int width, int height);

		void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
			window::width = glm::max(width, 1);
			window::height = glm::max(height, 1);

			if (size_callback) size_callback(window, width, height);
		}

		void openWindow(const int width, const int height, const char* title, const bool resizable, const int samples, void(*_framebuffer_size_callback)(GLFWwindow* window, int width, int height)) {
			if (window_created) return;

			glfwInit();
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_SAMPLES, samples);
			glfwWindowHint(GLFW_RED_BITS, 16);
			glfwWindowHint(GLFW_GREEN_BITS, 16);
			glfwWindowHint(GLFW_BLUE_BITS, 16);
			glfwWindowHint(GLFW_ALPHA_BITS, 8);
			glfwWindowHint(GLFW_RESIZABLE, resizable);
			glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

			window::width = width;
			window::height = height;
			default_width = width;
			default_height = height;

			size_callback = _framebuffer_size_callback;

			GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
			if (window == NULL)
			{
				std::cerr << "Failed to create GLFW window" << std::endl;
				glfwTerminate();
				return;
			}
			glfwMakeContextCurrent(window);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				std::cerr << "Failed to initialize GLAD" << std::endl;
				return;
			}

			graphics_check_external();
			
			glEnable(GL_MULTISAMPLE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glfwSwapInterval(0);
			glfwSetCharCallback(window, flo::editor::character_callback);
			glfwSetScrollCallback(window, flo::scroll_callback);
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
			flo::editor::init_textinput();

			timer.reset();

			init_shader_defaults();
			init_default_textures();

			glfw_window = window;

			framebuffer_size_callback(window, width, height);

			window_created = true;

			framebuffer.bounds = glm::ivec4(0, 0, width, height);
			framebuffer.bind();

			graphics_check_error();
		}

		void setTitle(const std::string& title) {
			glfwSetWindowTitle(glfw_window, title.data());
		}

		void setFixedFramerate(const int _frames) {
			framerate = _frames;
		}

		void setDynamicFramerateTolerance(const float delay) {
			max_tolerance = delay;
		}

		void waitForFrame() {
			++frames;

			uint frames_required = (timer.stop().asSeconds() * framerate);

			if (std::abs((int)frames_required - (int)frames) > (framerate * 0.1 + 1)) {
				timer.reset();
				frames = 0;
				frames_required = 0;
			}

			while (frames > frames_required) {
				frames_required = (timer.stop().asSeconds() * framerate);
			}
		}

		float getDeltaTime() {
			if (delta_time < 0.00001) return 0.00001;
			return delta_time;
		}

		void dispose() {
			if (!window_created) return;

			glfwDestroyWindow(glfw_window);
			glfwTerminate();

			window_created = true;
		}

		void checkEvents() {
			glfwPollEvents();
			flo::update_input(glfw_window);
			float now = timer.stop();
			delta_time = now - last;
			last = now;
			if (delta_time > max_tolerance) delta_time = max_tolerance;
			framebuffer.bounds = glm::ivec4(0, 0, width, height);
		}

		void hideCursor() {
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

		bool isClosing() {
			return glfwWindowShouldClose(glfw_window);
		}

		void flush() {
			graphics_check_external();
			glFlush();
			glfwSwapBuffers(glfw_window);
			graphics_check_error();
		}

		void setIcon(const std::vector<TextureHandle>& textures) {
			std::vector<GLFWimage> images;
			for (int i = 0; i < textures.size(); ++i) {
				GLFWimage icon;
				icon.pixels = textures[i].data;
				icon.width = textures[i].width;
				icon.height = textures[i].height;
				images.push_back(icon);
			}
			glfwSetWindowIcon(glfw_window, textures.size(), images.data());
		}

		void setConsoleVisible(bool visible) {
			#ifdef _WIN32
			::ShowWindow(::GetConsoleWindow(), visible ? SW_SHOW : SW_HIDE);
			#endif
		}

		bool hasFocus() {
			return glfwGetWindowAttrib(glfw_window, GLFW_FOCUSED);
		}

		void screenshot(const std::string& filename) {
			u8* pixels = new u8[width * height * 3];

			glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

			TextureHandle tex;
			tex.width = width;
			tex.height = height;
			tex.data = pixels;
			tex.bytes_per_pixel = 3;
			tex.saveFile(filename);
			
			delete[] pixels;
		}

		void close() {
			glfwSetWindowShouldClose(glfw_window, 1);
		}

		bool isFullscreened() {
			return glfwGetWindowMonitor(glfw_window) != nullptr;
		}

		void setFullscreen(bool fullscreen) {
			if (fullscreen == isFullscreened()) return;
			if (fullscreen) {
				previous_dimensions = glm::ivec2(width, height);

				const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

				glfwSetWindowMonitor(glfw_window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
			}
			else {
				const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

				glfwSetWindowMonitor(glfw_window, nullptr, mode->width / 2 - previous_dimensions.x / 2, mode->height / 2 - previous_dimensions.y / 2, previous_dimensions.x, previous_dimensions.y, 0);
			}
		}

		void clear(glm::vec3 color) {
			graphics_check_external();
			glClearDepth(1.0);
			glClearColor(color.r, color.g, color.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			graphics_check_error();
		}
	}
}