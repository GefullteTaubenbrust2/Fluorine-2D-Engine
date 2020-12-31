#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "../logic/Types.h"

namespace fgr {
#define set_uniform(shader, type, args) glUniform ## type (shader.shader_program, args)

    ///<summary>
    ///A struct for handling OpenGL shaders and their uniform variables.
    ///</summary>
	struct Shader {
        ///<summary>
        ///The ID of the named program object. WARNING: read-only!
        ///</summary>
		uint shader_program, vertex_shader, fragment_shader, geometry_shader;
        
        ///<summary>
        ///The location of the "transformations" uniform that should be present in the shader, else UB might result. WARNING: read-only!
        ///</summary>
		uint transformations_uniform;
        
        ///<summary>
        ///Is the Shader loaded? WARNING: read-only!
        ///</summary>
		bool loaded = false;

        ///<summary>
        ///Various default shaders for simple rendering. Find the appropriate Shader below:
        ///<list type="bullet">
        ///<item><description><para><em>basic: For VertexArrays with a flat color or gradient.</em></para></description></item>
        ///<item><description><para><em>basic_instanced: For InstanceArrays with a flat color or gradient.</em></para></description></item>
        ///<item><description><para><em>line: For VertexArrays with a flat color or gradient in lines with thickness.</em></para></description></item>
        ///<item><description><para><em>basic_instanced: For InstanceArrays with a flat color or gradient in lines with thickness.</em></para></description></item>
        ///<item><description><para><em>textured: For VertexArrays with a texture.</em></para></description></item>
        ///<item><description><para><em>textured_instanced: For InstanceArrays with a texture.</em></para></description></item>
        ///<item><description><para><em>sprites_instanced: For SpriteArrays.</em></para></description></item>
        ///<item><description><para><em>sprites_instanced_depth: currently deprecated.</em></para></description></item>
        ///</list>
        ///</summary>
		static Shader basic, basic_instanced, line, line_instanced, textured, textured_instanced, sprites_instanced, sprites_instanced_depth, lit_3d;

        ///<summary>
        ///A vector containing the uniform locations within the shader, the index is equal to the index of the appropriate name given on construction. 
        /// WARNING: read-only
        ///</summary>
		std::vector<uint> uniform_locations;

		Shader() = default;
        
        ///<summary>
        ///Load a shader with a vertex and fragment shader.
        ///</summary>
        ///<param name="vertex_path">The path of the vertex shader source file.</param>
        ///<param name="fragment_path">The path of the fragment shader source file.</param>
        ///<param name="uniforms">The names of all accessible uniform variables.</param>
        ///<returns>The success, false being a success.</returns>
        bool loadFromFile(const std::string& vertex_path, const std::string& fragment_path, const std::vector<std::string>& uniforms = std::vector<std::string>());

        ///<summary>
         ///Load a shader with a vertex, geometry and fragment shader.
        ///</summary>
        ///<param name="vertex_path">The path of the vertex shader source file.</param>
        ///<param name="geometry_path">The path of the geometry shader source file.</param>
        ///<param name="fragment_path">The path of the fragment shader source file.</param>
        ///<param name="uniforms">The names of all accessible uniform variables.</param>
        ///<returns>The success, false being a success.</returns>
		bool loadFromFile(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path, const std::vector<std::string>& uniforms = std::vector<std::string>());

        ///<summary>
        ///Destroy the shader programs.
        ///</summary>
		void clear();

        ///<summary>
        ///Bind the shader. This is NOT neccessary for rendering.
        ///</summary>
		void use();

        ///<summary>
        ///Set a uniform int.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
		void setInt(const int location, const int value);

        ///<summary>
        ///Set a uniform float.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
		void setFloat(const int location, const float value);

        ///<summary>
        ///Set a uniform double.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
		void setDouble(const int location, const double value);

        ///<summary>
        ///Set a uniform vec2.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
		void setVec2(const int location, const glm::vec2 value);

        ///<summary>
        ///Set a uniform vec3.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
		void setVec3(const int location, const glm::vec3 value);

        ///<summary>
        ///Set a uniform vec4.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
		void setVec4(const int location, const glm::vec4 value);

        ///<summary>
        ///Set a uniform 3x3 matrix.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
        ///<param name="transpose">Transpose the matrix before setting the value?.</param>
		void setMat3(const int location, const glm::mat3 value, const bool transpose = false);

        ///<summary>
        ///Set a uniform 4x4 matrix.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">The desired value to set the uniform to.</param>
        ///<param name="transpose">Transpose the matrix before setting the value?.</param>
		void setMat4(const int location, const glm::mat4 value, const bool transpose = false);

        ///<summary>
        ///Set a uniform array of ints.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
		void setIntArray(const int location, const int* value, const int count);

        ///<summary>
        ///Set a uniform array of floats.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
		void setFloatArray(const int location, const float* value, const int count);

        ///<summary>
        ///Set a uniform array of doubles.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
		void setDoubleArray(const int location, const double* value, const int count);

        ///<summary>
        ///Set a uniform array of 2D vectors.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
		void setVec2Array(const int location, const glm::vec2* value, const int count);

        ///<summary>
        ///Set a uniform array of 3D vectors.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
		void setVec3Array(const int location, const glm::vec3* value, const int count);

        ///<summary>
        ///Set a uniform array of 4D vectors.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
		void setVec4Array(const int location, const glm::vec4* value, const int count);

        ///<summary>
        ///Set a uniform array of 3x3 matrices.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
        ///<param name="transpose">Transpose the matrices before setting the values?.</param>
		void setMat3Array(const int location, const glm::mat3* value, const int count, const bool transpose = false);

        ///<summary>
        ///Set a uniform array of 4x4 matrices.
        ///</summary>
        ///<param name="location">The index of the location in the "uniform_locations" vector. Assume the uniform "foo" has been named as the first element for the vector, then location shall be 0 for setting the uniform.</param>
        ///<param name="value">A pointer to the desired values.</param>
        ///<param name="value">The amount of elements in the array. Should be equal for "value" and the in-shader uniform variable.</param>
        ///<param name="transpose">Transpose the matrices before setting the values?.</param>
		void setMat4Array(const int location, const glm::mat4* value, const int count, const bool transpose = false);
	};
}
