#include "ComputeShader.h"

#include "GErrorHandler.h"

#include <fstream>

namespace fgr {
	bool ComputeShader::loadFromFile(const std::string& path, const uint shader_storage_object_count, glm::uvec3 global_invocation_size, const std::vector<std::string>& uniforms) {
		graphics_check_external();

		std::ifstream stream;
		stream.open(path, std::ios::ate | std::ios::binary);
		stream.seekg(0, std::ios::end);
		uint size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		char* compute_src = new char[size + 1];
		stream.read(compute_src, size);
		compute_src[size] = '\0';

		compute_shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(compute_shader, 1, &compute_src, NULL);
		glCompileShader(compute_shader);
		int success;
		char infoLog[512];
		glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(compute_shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		stream.close();
		delete[] compute_src;

		shader_program = glCreateProgram();
		glAttachShader(shader_program, compute_shader);
		glLinkProgram(shader_program);
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(compute_shader);

		ssbo_count = shader_storage_object_count;
		shader_storage_objects = new uint[ssbo_count];
		glGenBuffers(ssbo_count, shader_storage_objects);

		uniform_count = uniforms.size();
		uniform_locations = new uint[uniform_count];
		for (int i = 0; i < uniform_count; ++i) {
			uniform_locations[i] = glGetUniformLocation(shader_program, uniforms[i].data());
		}
		
		loaded = true;

		ComputeShader::global_invocation_size = global_invocation_size;

		glm::ivec3 workgroup_size = glm::ivec3(1);

		work_group_count = glm::vec3(
			global_invocation_size.x / workgroup_size.x + (global_invocation_size.x % workgroup_size.x == 0 ? 0 : 1),
			global_invocation_size.y / workgroup_size.y + (global_invocation_size.y % workgroup_size.y == 0 ? 0 : 1),
			global_invocation_size.z / workgroup_size.z + (global_invocation_size.z % workgroup_size.z == 0 ? 0 : 1));

		graphics_check_error();

		return 0;
	}

	void ComputeShader::dispatch() {
		graphics_check_external();

		for (int i = 0; i < ssbo_count; ++i) {
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, shader_storage_objects[i]);
		}

		glUseProgram(shader_program);
		glDispatchCompute(work_group_count.x, work_group_count.y, work_group_count.z);

		glFinish();

		graphics_check_error();
	}

	void ComputeShader::clear() {
		graphics_check_external();

		if (shader_program) glDeleteProgram(shader_program);
		glDeleteBuffers(ssbo_count, shader_storage_objects);
		delete[] shader_storage_objects;
		shader_storage_objects = nullptr;
		delete[] uniform_locations;
		uniform_locations = nullptr;

		graphics_check_error();
	}
}