#pragma once
#include "FrameBuffer.h"

namespace fgr {
	///<summary>
	/// A struct for blurring images.
	///</summary>
	struct BlurBuffer {
	protected:
		FrameBuffer io_fbo, util_fbo;
		VertexArray va;
	public:
		///<summary>
		/// The radius of the blur in pixels.
		///</summary>
		float blur_radius = 10.;

		///<summary>
		/// When set to true, the quality will be improved. This will slow down the blurring process.
		///</summary>
		bool increased_blur = false;
		
		BlurBuffer() = default;

		///<summary>
		/// Construct a BlurBuffer with a given radius. The default constructor too will suffice.
		///</summary>
		///<param name="blur_radius">The radius of the blur in pixels.</param>
		BlurBuffer(float blur_radius);

		///<summary>
		/// Must be called before blurring.
		///</summary>
		void init();

		///<summary>
		/// Blur the contents of the fbo. Note that it will remain unchanged but the generated output can still be accessed.
		///</summary>
		///<param name="fbo">A reference to the fbo to bind.</param>
		void blur(FrameBuffer& fbo);

		///<summary>
		/// Bind the blurred texture.
		///</summary>
		///<param name="unit">The texture unit to bind to.</param>
		void bindContent(fgr::TextureUnit unit);

		///<summary>
		/// Destroy all allocated contents.
		///</summary>
		void dispose();
	};
}