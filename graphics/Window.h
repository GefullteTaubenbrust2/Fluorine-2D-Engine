#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../logic/Matrices.h"

#include <iostream>

#include <vector>

namespace fgr {
	struct TextureHandle;

	namespace window {
        ///<summary>
        ///The current window dimensions.
        ///</summary>
		extern int width, height;

        ///<summary>
        ///Open a new window. There can only be one window.
        ///</summary>
        ///<param name="width">The initial width.</param>
        ///<param name="height">The initial height.</param>
        ///<param name="title">The initial title.</param>
        ///<param name="resizable">Self-explanatory.</param>
        ///<param name="samples">When above 1, this value specifies the amount of samples to be used with multisampling.</param>
        ///<param name="framebuffer_size_callback">Is called when the window is resized.</param>
		void openWindow(const int width, const int height, const char* title, const bool resizable, const int samples, void(*framebuffer_size_callback)(GLFWwindow* window, int width, int height) = nullptr);

        ///<summary>
        ///Set the window's title.
        ///</summary>
        ///<param name="title">The new title for the window.</param>
		void setTitle(const std::string& title);

        ///<summary>
        ///Set the framerate limit. It is not automatically applied.
        ///</summary>
        ///<param name="frames">The highest possible framerate.</param>
		void setFixedFramerate(const int frames);

        ///<summary>
        ///Set the greatest amount of time to pass between frames before lag is accepted.
        ///</summary>
        ///<param name="delay">The highest tolerated delay.</param>
		void setDynamicFramerateTolerance(const float delay);

        ///<summary>
        ///Wait for the next frame based on the set maximum framerate.
        ///</summary>
		void waitForFrame();

        ///<summary>
        ///Get the time between frames.
        ///</summary>
		float getDeltaTime();

        ///<summary>
        ///Check the GLFW events, input, delta time etc..
        ///</summary>
		void checkEvents();

        ///<summary>
        ///Is the window closing?
        ///</summary>
		bool isClosing();

		void hideCursor();

        ///<summary>
        ///Clear the window's color.
        ///</summary>
        ///<param name="color">The clear color.</param>
		void clear(glm::vec3 color);

        ///<summary>
        ///Bring the drawn contents to the screen.
        ///</summary>
		void flush();

		void setIcon(std::vector<TextureHandle> textures);

		void setConsoleVisible(bool visible);

		void setFullscreen(bool fullscreen);

		bool isFullscreened();

		bool hasFocus();

		void close();

        ///<summary>
        ///Destroy the OpenGL context and the window.
        ///</summary>
		void dispose();
	}
}
