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

	struct RenderTarget;

	namespace window {
        ///<summary>
        ///The current window dimensions.
        ///</summary>
		extern int width, height;

		///<summary>
		/// The window's RenderTarget.
		///</summary>
		extern RenderTarget framebuffer;

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

		///<summary>
		/// Irreversibly hide the cursor.
		///</summary>
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

		///<summary>
		/// Set the window icon.
		///</summary>
		///<param name="textures">A selection of textures that can be selected depending on the ideal size.</param>
		void setIcon(const std::vector<TextureHandle>& textures);

		///<summary>
		/// Hide or show the console.
		///</summary>
		///<param name="visible">Should the console be shown?</param>
		void setConsoleVisible(bool visible);

		///<summary>
		/// Bring the window into or out of fullscreen mode.
		///</summary>
		///<param name="fullscreen">Enable or disable fullscreen mode.</param>
		void setFullscreen(bool fullscreen);

		///<summary>
		/// Is the window fullscreened?
		///</summary>
		///<returns>Is the window fullscreened?</returns>
		bool isFullscreened();

		///<summary>
		/// Is the window in focus?
		///</summary>
		///<returns>Is the window in focus?</returns>
		bool hasFocus();

		///<summary>
		/// Take a screenshot of the window's contents.
		///</summary>
		///<param name="path">The path of the image file to write to.</param>
		void screenshot(const std::string& path);

		///<summary>
		/// Request to close the window.
		///</summary>
		void close();

        ///<summary>
        ///Destroy the OpenGL context and the window.
        ///</summary>
		void dispose();
	}
}
