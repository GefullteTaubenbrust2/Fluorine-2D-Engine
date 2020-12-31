#pragma once
#include "Matrices.h"
#include "../graphics/FrameBuffer.h"

namespace flo {
	///<summary>
	/// A struct for Creating pixel perfect views.
	///</summary>
	struct View {
	public:
		///<summary>
		/// The position of the view's center.
		///</summary>
		glm::vec2 position = glm::vec2(0.);

		///<summary>
		/// The transform matrices to use for applying the view.
		///</summary>
		glm::mat3 transform, transform_inverse;

	protected:
		ScaleMode scalemode;
		fgr::RenderTarget* target = nullptr;
		glm::vec2 initial_mouse_pos, initial_pos;
		int pixelsize = 1;

	public:
		View() = default;

		///<summary>
		/// Construct a view.
		///</summary>
		///<param name="scale_method">The method of generating a scale.</param>
		///<param name="rounding">The rounding method to use for scaling.</param>
		///<param name="base_size">The screen size at which the scale is 1:1.</param>
		///<param name="target">The RenderTarget to generate the view from.</param>
		View(ScaleModes scale_method, ScaleModes rounding, int base_size, fgr::RenderTarget* target);

		///<summary>
		/// Update the matrices.
		///</summary>
		void updateViewMatrix();

		///<summary>
		/// When called, the cursor can be used to drag the view around.
		///</summary>
		void cursorDrag();
	};
}