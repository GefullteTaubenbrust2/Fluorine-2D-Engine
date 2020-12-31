#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"

namespace fgr {
	///<summary>
	/// Any framebuffer has a rendertarget, which is used to bind it.
	///</summary>
	struct RenderTarget {
		uint id = 0;
		glm::ivec4 bounds = glm::ivec4(0);

		///<summary>
		///The currently bound FBO (any kind of FBO).
		///WARNING read-only!
		///</summary>
		static RenderTarget bound;

		RenderTarget() = default;

		void bind();

		static void unbind();
	};

    ///<summary>
    ///A struct for creating and handling OpenGL framebuffers. 
    ///A texture will be supplied along with the FBO itself by default.
    ///</summary>
	struct FrameBuffer {
        ///<summary>
        ///The appropriate IDs of the various OpenGL objects associated with and including the FBO itself are contained in these variables.
        ///WARNING read-only!
        ///</summary>
		uint fbo = 0, texture = 0, rbo = 0;
        
        ///<summary>
        ///Format is used to specify if i.e. 16 or 8 bit color is to be used.
        ///Wrap and filter are variables regarding the associated texture.
        ///WARNING read-only!
        ///</summary>
		uint format = GL_RGBA, wrap, filter;
        
        ///<summary>
        ///The dimensions of the associated texture.
        ///WARNING read-only!
        ///</summary>
		int tex_width, tex_height;
        
        ///<summary>
        ///Have the OpenGL objects been created?
        ///WARNING read-only!
        ///</summary>
		bool inited = false;
        
		RenderTarget target;
		
        ///<summary>
        ///Initialize the object and create the OpenGL objects required for usage.
        ///</summary>
        ///<param name="width">Width of the associated texture. May later be modified.</param>
        ///<param name="heigh">Height of the associated texture. May later be modified.</param>
        ///<param name="format">Format is used to specify if i.e. 16 or 8 bit color is to be used.</param>
        ///<param name="wrap">The texture's OpenGL wrap mode.</param>
        ///<param name="filter">The texture's OpenGL filter mode.</param>
		void init(const int width, const int height, const uint format, const uint wrap, const uint filter);

        ///<summary>
        ///Modify the output texture's size.
        ///</summary>
        ///<param name="width">New width of the associated texture.</param>
        ///<param name="heigh">New height of the associated texture.</param>
		void resize(const int width, const int height);

		///<summary>
	    ///Used for copying the textures content to a different frambuffer where it can be used. Linear blending will be used when scaling.
	    ///</summary>
	    ///<param name="output_fbo">The FBO to write to.</param>
		void resolve(FrameBuffer& fbo);
			
        ///<summary>
        ///Bind the output texture to an OpenGL texture unit.
        ///</summary>
        ///<param name="unit">The target unit.</param>
		void bindContent(const TextureUnit unit);

        ///<summary>
        ///Bind the FBO to the current OpenGL context for rendering.
        ///</summary>
		void bind();

        ///<summary>
        ///Unbind any and all bound FBOs of the current OpenGL context.
        ///</summary>
		void unbind();

        ///<summary>
        ///Destroys the FBO and all associated data, aside from (when specified) the texture it draws to.
        ///</summary>
        ///<param name="clearTexture">Should the texture be kept?</param>
		void dispose(bool clearTexture);

        ///<summary>
        ///Clear the texture's color.
        ///</summary>
        ///<param name="color">The color with which to fill the texture.</param>
		void clear(const glm::vec4& color);
	};

    ///<summary>
    ///A struct for creating and handling OpenGL framebuffers with multisampling.
    ///</summary>
	struct FrameBufferMS {
        ///<summary>
        ///The appropriate IDs of the various OpenGL objects associated with and including the FBO itself are contained in these variables. The texure is not to be used elsewhere.
        ///WARNING read-only!
        ///</summary>
		uint fbo = 0, texture = 0, depth_tex = 0;
        
        ///<summary>
        ///The dimensions of the associated texture.
        ///WARNING read-only!
        ///</summary>
		int tex_width, tex_height;
        
        ///<summary>
        ///Format is used to specify if i.e. 16 or 8 bit color is to be used.
        ///WARNING read-only!
        ///</summary>
		uint format = GL_RGBA;
        
        ///<summary>
        ///Have the OpenGL objects been created?
        ///WARNING read-only!
        ///</summary>
		bool inited = false;

		RenderTarget target;

        ///<summary>
        ///Initialize the object and create the OpenGL objects required for usage.
        ///</summary>
        ///<param name="width">Width of the associated texture. May later be modified.</param>
        ///<param name="heigh">Height of the associated texture. May later be modified.</param>
        ///<param name="format">Format is used to specify if i.e. 16 or 8 bit color is to be used.</param>
		void init(const int width, const int height, const uint format);

        ///<summary>
        ///Used for copying the textures content to a regular frambuffer where it can be used.
        ///</summary>
        ///<param name="output_fbo">ID of a regular FBO to write to.</param>
		void resolve(const uint output_fbo);

		///<summary>
        ///Modify the output texture's size.
        ///</summary>
        ///<param name="width">New width of the associated texture.</param>
        ///<param name="heigh">New height of the associated texture.</param>
		void resize(const int width, const int height);

        ///<summary>
        ///Bind the FBO to the current OpenGL context for rendering.
        ///</summary>
		void bind();

        ///<summary>
        ///Unbind any and all bound FBOs of the current OpenGL context.
        ///</summary>
		void unbind();

        ///<summary>
        ///Destroys the FBO and all associated data.
        ///</summary>
		void dispose();

        ///<summary>
        ///Clear the texture's color.
        ///</summary>
        ///<param name="color">The color with which to fill the texture.</param>
		void clear(const glm::vec4& color);
	};
}
