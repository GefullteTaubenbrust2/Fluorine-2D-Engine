#include <fstream>

#include "Shader.h"

#include "GErrorHandler.h"

namespace fgr {
	Shader Shader::basic, Shader::basic_instanced, Shader::line, Shader::line_instanced, Shader::textured, Shader::textured_instanced, Shader::sprites_instanced, Shader::sprites_instanced_depth;

	void init_shader_defaults() {
		Shader::basic.loadFromFile("src/shaders/basic.vert", "src/shaders/basic.frag", std::vector<std::string>{});
		Shader::basic_instanced.loadFromFile("src/shaders/instanced/basic.vert", "src/shaders/basic.frag", std::vector<std::string>{});
		Shader::line.loadFromFile("src/shaders/line.vert", "src/shaders/basic.frag", "src/shaders/line.geom", std::vector<std::string>{"screen_dimensions", "line_thickness"});
		Shader::line_instanced.loadFromFile("src/shaders/instanced/line.vert", "src/shaders/basic.frag", "src/shaders/line.geom", std::vector<std::string>{"screen_dimensions", "line_thickness"});
		Shader::textured.loadFromFile("src/shaders/textured.vert", "src/shaders/textured.frag", std::vector<std::string>{"texture"});
		Shader::textured_instanced.loadFromFile("src/shaders/instanced/textured.vert", "src/shaders/textured.frag", std::vector<std::string>{"texture"});
		Shader::sprites_instanced.loadFromFile("src/shaders/instanced/sprite.vert", "src/shaders/instanced/sprite.frag", std::vector<std::string>{"textures"});
		Shader::sprites_instanced_depth.loadFromFile("src/shaders/instanced/sprite.vert", "src/shaders/instanced/depthsprite.frag", std::vector<std::string>{"textures"});
	}

	bool Shader::loadFromFile(const std::string& vertex_path, const std::string& fragment_path, std::vector<std::string> uniforms) {
		graphics_check_external();

		std::ifstream stream;
		stream.open(vertex_path, std::ios::ate | std::ios::binary);
		stream.seekg(0, std::ios::end);
		unsigned int size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		char* vertex_src = new char[size + 1];
		stream.read(vertex_src, size);
		vertex_src[size] = '\0';

		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_src, NULL);
		glCompileShader(vertex_shader);
		int success;
		char infoLog[512];
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		stream.close();
		delete[] vertex_src;

		stream.open(fragment_path, std::ios::ate | std::ios::binary);
		stream.seekg(0, std::ios::end);
		size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		char* fragment_src = new char[size + 1];
		stream.read(fragment_src, size);
		fragment_src[size] = '\0';

		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_src, NULL);
		glCompileShader(fragment_shader);
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		stream.close();
		delete[] fragment_src;

		shader_program = glCreateProgram();
		glAttachShader(shader_program, vertex_shader);
		glAttachShader(shader_program, fragment_shader);
		glLinkProgram(shader_program);
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		uniform_locations.resize(uniforms.size());
		for (int i = 0; i < uniforms.size(); ++i) {
			uniform_locations[i] = glGetUniformLocation(shader_program, uniforms[i].data());
		}
		transformations_uniform = glGetUniformLocation(shader_program, "transformations");

		loaded = true;

		graphics_check_error();

		return 0;
	}

	bool Shader::loadFromFile(const std::string& vertex_path, const std::string& fragment_path, const std::string& geometry_path, std::vector<std::string> uniforms) {
		graphics_check_external();

		std::ifstream stream;
		stream.open(vertex_path, std::ios::ate | std::ios::binary);
		stream.seekg(0, std::ios::end);
		unsigned int size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		char* vertex_src = new char[size + 1];
		stream.read(vertex_src, size);
		vertex_src[size] = '\0';

		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &vertex_src, NULL);
		glCompileShader(vertex_shader);
		int success;
		char infoLog[512];
		glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		stream.close();
		delete[] vertex_src;

		stream.open(fragment_path, std::ios::ate | std::ios::binary);
		stream.seekg(0, std::ios::end);
		size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		char* fragment_src = new char[size + 1];
		stream.read(fragment_src, size);
		fragment_src[size] = '\0';

		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &fragment_src, NULL);
		glCompileShader(fragment_shader);
		glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		stream.close();
		delete[] fragment_src;

		stream.open(geometry_path, std::ios::ate | std::ios::binary);
		stream.seekg(0, std::ios::end);
		size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		char* geometry_src = new char[size + 1];
		stream.read(geometry_src, size);
		geometry_src[size] = '\0';

		geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry_shader, 1, &geometry_src, NULL);
		glCompileShader(geometry_shader);
		glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry_shader, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		stream.close();
		delete[] geometry_src;

		shader_program = glCreateProgram();
		glAttachShader(shader_program, vertex_shader);
		glAttachShader(shader_program, fragment_shader);
		glAttachShader(shader_program, geometry_shader);
		glLinkProgram(shader_program);
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteShader(geometry_shader);

		uniform_locations.resize(uniforms.size());
		for (int i = 0; i < uniforms.size(); ++i) {
			uniform_locations[i] = glGetUniformLocation(shader_program, uniforms[i].data());
		}
		transformations_uniform = glGetUniformLocation(shader_program, "transformations");

		loaded = true;

		graphics_check_error();

		return 0;
	}

	void Shader::clear() {
		graphics_check_external();
		if (shader_program) glDeleteProgram(shader_program);
		graphics_check_error();
	}

	void Shader::use() {
		graphics_check_external();
		glUseProgram(shader_program);
		graphics_check_error();
	}

	void Shader::setInt(const int location, const int value) {
		glUseProgram(shader_program);
		glUniform1i(uniform_locations[location], value);
	}

	void Shader::setFloat(const int location, const float value) {
		glUseProgram(shader_program);
		glUniform1f(uniform_locations[location], value);
	}

	void Shader::setDouble(const int location, const double value) {
		glUseProgram(shader_program);
		glUniform1d(uniform_locations[location], value);
	}

	void Shader::setVec2(const int location, const glm::vec2 value) {
		glUseProgram(shader_program);
		glUniform2fv(uniform_locations[location], 1, glm::value_ptr(value));
	}

	void Shader::setVec3(const int location, const glm::vec3 value) {
		glUseProgram(shader_program);
		glUniform3fv(uniform_locations[location], 1, glm::value_ptr(value));
	}

	void Shader::setVec4(const int location, const glm::vec4 value) {
		glUseProgram(shader_program);
		glUniform4fv(uniform_locations[location], 1, glm::value_ptr(value));
	}

	void Shader::setMat3(const int location, const glm::mat3 value, const bool transpose) {
		glUseProgram(shader_program);
		glUniformMatrix3fv(uniform_locations[location], 1, transpose, glm::value_ptr(value));
	}

	void Shader::setMat4(const int location, const glm::mat4 value, const bool transpose) {
		glUseProgram(shader_program);
		glUniformMatrix4fv(uniform_locations[location], 1, transpose, glm::value_ptr(value));
	}

	void Shader::setIntArray(const int location, const int* value, const int count) {
		glUseProgram(shader_program);
		glUniform1iv(uniform_locations[location], count, value);
	}

	void Shader::setFloatArray(const int location, const float* value, const int count) {
		glUseProgram(shader_program);
		glUniform1fv(uniform_locations[location], count, value);
	}

	void Shader::setDoubleArray(const int location, const double* value, const int count) {
		glUseProgram(shader_program);
		glUniform1dv(uniform_locations[location], count, value);
	}

	void Shader::setVec2Array(const int location, const glm::vec2* value, const int count) {
		glUseProgram(shader_program);
		glUniform2fv(uniform_locations[location], count, glm::value_ptr(value[0]));
	}

	void Shader::setVec3Array(const int location, const glm::vec3* value, const int count) {
		glUseProgram(shader_program);
		glUniform3fv(uniform_locations[location], count, glm::value_ptr(value[0]));
	}

	void Shader::setVec4Array(const int location, const glm::vec4* value, const int count) {
		glUseProgram(shader_program);
		glUniform4fv(uniform_locations[location], count, glm::value_ptr(value[0]));
	}

	void Shader::setMat3Array(const int location, const glm::mat3* value, const int count, const bool transpose) {
		glUseProgram(shader_program);
		glUniformMatrix3fv(uniform_locations[location], count, transpose, glm::value_ptr(value[0]));
	}

	void Shader::setMat4Array(const int location, const glm::mat4* value, const int count, const bool transpose) {
		glUseProgram(shader_program);
		glUniformMatrix4fv(uniform_locations[location], count, transpose, glm::value_ptr(value[0]));
	}
}