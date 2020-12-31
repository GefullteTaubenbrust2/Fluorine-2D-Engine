#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "../logic/Types.h"

namespace fgr {
    ///<summary>
    ///A struct for creating and handling OpenGL compute shaders.
    ///</summary>
	struct ComputeShader {
        
        ///<summary>
        ///IDs of the program and the shader itself, respectively. 
        ///WARNING read-only!
        ///</summary>
		uint shader_program, compute_shader;
        
        ///<summary>
        ///Has the shader yet been loaded? 
        ///WARNING read-only!
        ///</summary>
		bool loaded;

        ///<summary>
        ///WARNING read-only!
        ///</summary>
		uint *shader_storage_objects, *uniform_locations;
        
        ///<summary>
        ///WARNING read-only!
        ///</summary>
		uint ssbo_count, uniform_count;

        ///<summary>
        ///The number of workgroups for all axes. 
        ///WARNING read-only!
        ///</summary>
		glm::uvec3 work_group_count;
        
        ///<summary>
        ///The invocation size for all axes.
        ///WARNING read-only!
        ///</summary>
		glm::uvec3 global_invocation_size;

		ComputeShader() = default;

        ///<summary>
        ///Load a  compute shader from corresponding files and create the appropriate data for it.
        ///</summary>
        ///<param name="path">The path of the compute shader source file.</param>
        ///<param name="shader_storage_object_count">The number of SSBOs associated to the shader.</param>
        ///<param name="global_invocation_size">The global invocation size, as a vector.</param>
        ///<param name="uniforms">A vector containing the names of all uniform variables.</param>
        ///<returns>The success, false being a success.</returns>
		bool loadFromFile(const std::string& path, const uint shader_storage_object_count, glm::uvec3 global_invocation_size, const std::vector<std::string>& uniforms = std::vector<std::string>());

        ///<summary>
        ///Run the shader.
        ///</summary>
		void dispatch();

        ///<summary>
        ///Delete the shader and associated data in the struct.
        ///</summary>
		void clear();
	};
}
