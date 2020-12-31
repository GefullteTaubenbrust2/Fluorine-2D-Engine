#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace flo {
    ///<summary>
    ///Scale a matrix.
    ///</summary>
    ///<param name="matrix">The matrix to scale.</param>
    ///<param name="scale">The scale as a 2D vector.</param>
    ///<returns>The resulting matrix.</returns>
	glm::mat3 scale(const glm::mat3& matrix, glm::vec2 scale);

    ///<summary>
    ///Rotate a matrix by an angle.
    ///</summary>
    ///<param name="matrix">The matrix to rotate.</param>
    ///<param name="angle">The angle.</param>
    ///<returns>The resulting matrix.</returns>
	glm::mat3 rotate(const glm::mat3& matrix, const float angle);

    ///<summary>
    ///Rotate a matrix with given sin and cos values.
    ///</summary>
    ///<param name="matrix">The matrix to rotate.</param>
    ///<param name="sin">The sine of the angle.</param>
    ///<param name="cos">The cosine of the angle.</param>
    ///<returns>The resulting matrix.</returns>
	glm::mat3 rotate(const glm::mat3& matrix, const float cos, const float sin);

    ///<summary>
    ///Translate a matrix.
    ///</summary>
    ///<param name="matrix">The matrix to translate.</param>
    ///<param name="offset">The offset by which to translate.</param>
    ///<returns>The resulting matrix.</returns>
	glm::mat3 translate(const glm::mat3& matrix, glm::vec2 offset);

    ///<summary>
    ///Scale and then translate a space.
    ///</summary>
    ///<param name="matrix">The matrix to translate.</param>
    ///<param name="offset">The offset by which to translate.</param>
    ///<param name="scale">The scale as a 2D vector.</param>
    ///<returns>The resulting matrix.</returns>
	glm::mat3 scale_and_translate(const glm::vec2& scale, const glm::vec2& offset);

	///<summary>
	/// Scale, then rotate, then translate a space.
	///</summary>
	///<param name="scale">The scale.</param>
	///<param name="offset">The offest.</param>
	///<param name="direction">For an angle a of rotation, this should be [cos(a), sin(a)].</param>
	///<returns>The resulting matrix.</returns>
	glm::mat3 scale_rotate_and_translate(const glm::vec2& scale, const glm::vec2& offset, const glm::vec2& direction);

	///<summary>
	/// Translate, then scale, then rotate, then again translate a space.
	///</summary>
	///<param name="ioffset">The frist offest.</param>
	///<param name="scale">The scale.</param>
	///<param name="offset">The second offest.</param>
	///<param name="direction">For an angle a of rotation, this should be [cos(a), sin(a)].</param>
	///<returns>The resulting matrix.</returns>
	glm::mat3 generate_tsrt_matrix(const glm::vec2& ioffset, const glm::vec2& scale, const glm::vec2& direction, const glm::vec2& offset);

	///<summary>
	/// This matrix will now scale the z-component of a 3D vector.
	///</summary>
	///<param name="matrix">The matrix to apply the transformation to.</param>
	///<param name="depth">The depth value to use.</param>
	///<returns>The resulting matrix.</returns>
	glm::mat3 setDepth(glm::mat3 matrix, float depth);

    ///<summary>
    ///Different parameters for scaling according to frambeuffer size.
    ///</summary>
	enum ScaleModes {
		no_rounding = 0,
		scale_with_width = 0,
		scale_with_height = 1,
		scale_with_diagonal = 2,
		scale_with_largest = 3,
		scale_with_smallest = 4,
		constant_scale = 5,
		round_to_nearest = 16,
		round_to_smallest = 32,
		round_to_largest = 48,
	};

    ///<summary>
    ///A struct containing information about how to generate a scale from framebuffer dimensions. All variables may be changed. 
    ///In relation to this struct, the "scaling factor" will describe the factor by which a 2D object will be scaled when the framebuffer size is changed from its default.
    ///The "raw" scaling factor is the scaling factor before multiplying by "base_scale". The "actual" scaling factor will be applied in actuality.
    ///</summary>
	struct ScaleMode {
		ScaleModes mode, rounding;
		short base_dimension;
		float base_scale;
		char pixel_size;

		ScaleMode() = default;

        ///<summary>
        ///Create an instance of the struct.
        ///</summary>
        ///<param name="mode">Specify what should change the scaling factor.</param>
        ///<param name="rounding">When rounding is applied, the scaling factor (before multiplying by "base_scale") will be rounded to an integer.</param>
        ///<param name="base_width">The width at which the raw scaling factor, based on the width, is 1.</param>
        ///<param name="base_height">The height at which the raw scaling factor, based on the height, is 1.</param>
        ///<param name="base_scale">The raw scaling factor is multiplied by this value, yielding the actual scaling factor.</param>
        ///<param name="pixel_size">-deprecated, leave as its default-.</param>
		ScaleMode(ScaleModes mode, ScaleModes rounding, short base_dimension = 0, float base_scale = 1., char pixel_size = 1);
	};

    ///<summary>
    ///Generate a scaling factor based on the framebuffer's size. A squaure, even when the buffer is resized, will remain a square through this factor.
    ///It also ensures that the size of said square scales correctly.
    ///</summary>
    ///<param name="width">The framebuffer's width.</param>
    ///<param name="height">The framebuffer's height.</param>
    ///<param name="scalemode">The scalemode that will be used.</param>
    ///<returns>The resulting scaling factor as a 2D vector. Use for scaling the transformation matrix.</returns>
	glm::vec2 applyWindowScale(const int width, const int height, const ScaleMode& scalemode);

    ///<summary>
    ///Generate an offset for the transformation matrix for pixel perfection when scaling.
    ///</summary>
    ///<param name="width">The framebuffer's width.</param>
    ///<param name="height">The framebuffer's height.</param>
    ///<returns>The resulting offset as a 2D vector. Use for translating the transformation matrix.</returns>
	glm::vec2 centerWindowOrigin(const int width, const int height);

	///<summary>
	/// Let this function return a vector 'b', then the sum of a vector 'a' that lies perfectly on a pixel and 'b' too lies perfectly on a pixel, given that the normalized coordinate space is used.
	///</summary>
	///<param name="pos">A vector, to which 'b' will be as close as possible.</param>
	///<param name="width">The width of the framebuffer in pixels.</param>
	///<param name="height">The height of the framebuffer in pixels.</param>
	///<returns>The resulting vector 'b'.</returns>
	glm::vec2 fixToPixelPerfection(glm::vec2& pos, const int width, const int height);

    ///<summary>
    ///Get the size of what would normally be one pixel after scaled by the scaling factor, in pixels. 
    ///This is most usefull when using pixel perfect scales.
    ///</summary>
    ///<param name="width">The framebuffer's width.</param>
    ///<param name="height">The framebuffer's height.</param>
    ///<param name="scalemode">The scalemode that will be used.</param>
    ///<returns>The size of one pixel.</returns>
	int getPixelSize(const int width, const int height, const ScaleMode& scalemode);

	int fixPixelScale(int width, int pixel_size);
}
