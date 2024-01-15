#include "lith/shader.h"
#include "lith/log.h"
#include "gl/glad.h"
#include "lith/string.h"

ShaderProgram::ShaderProgram()
	: handle              (0)
	, sucessfullyCompiled (false)
{}

ShaderProgram::ShaderProgram(const ShaderProgramData& data)
	: handle              (0)
	, data                (data)
	, sucessfullyCompiled (false)
{}

ShaderProgram& ShaderProgram::compile() {
	if (!handle) {
		handle = glCreateProgram();
	}

	sucessfullyCompiled = true;

	for (ShaderSource& source : data.shaders) {
		if (!source.handle) {
			source.handle = glCreateShader(source.type);
		}

		glShaderSource(source.handle, 1, &source.source, NULL);
		glCompileShader(source.handle);
		glAttachShader(handle, source.handle);

		// check for compilation errors

		GLint isCompiled = 0;
		glGetShaderiv(source.handle, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_FALSE)
		{
			sucessfullyCompiled = false;

			glDeleteShader(source.handle);

			GLint maxLength = 0;
			glGetShaderiv(source.handle, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(source.handle, maxLength, &maxLength, &infoLog[0]);

			print("Failed to compile shader:");
			
			std::vector<std::string> lines = split(source.source, "\n");
			std::vector<std::string> errorMessage = split(infoLog.data(), "\n");

			std::vector<int> errorLines = {};

			for (std::string line : errorMessage) {
				int openParen = line.find_first_of('(');
				int closeParen = line.find_first_of(')');

				if (openParen == std::string::npos || closeParen == std::string::npos) {
					continue;
				}

				int lineIndex = stoi(line.substr(openParen + 1, closeParen - openParen)) - 1;
				errorLines.push_back(lineIndex);
			}

			for (int i = 0; i < errorLines.size(); i++) {
				const std::string& line = lines[errorLines[i]];
				const std::string& error = errorMessage[i];
				print("{}\n\n\t{}\n", error, trim(line));
			}

			free();
		}
	}

	glLinkProgram(handle);

	return *this;
}

void ShaderProgram::free() {
	glDeleteProgram(handle);
	handle = 0;
}

// can I get rid of all these ifs in a nice way?

bool ShaderProgram::use() {
	if (sucessfullyCompiled) {
		glUseProgram(handle);
	}

	return sucessfullyCompiled;
}

void ShaderProgram::dispatch(int x, int y, int z) {
	glDispatchCompute(x, y, z);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ShaderProgram::seti(const char* name, int i1) {
	glUniform1i(glGetUniformLocation(handle, name), i1);
}

void ShaderProgram::setf(const char* name, float f1) {
    glUniform1f(glGetUniformLocation(handle, name), f1);
}

void ShaderProgram::setf2(const char* name, const vec2& f2) {
	glUniform2f(glGetUniformLocation(handle, name), f2.x, f2.y);
}

void ShaderProgram::setf3(const char* name, const vec3& f3) {
	glUniform3f(glGetUniformLocation(handle, name), f3.x, f3.y, f3.z);
}

void ShaderProgram::setf4(const char* name, const vec4& f4) {
	glUniform4f(glGetUniformLocation(handle, name), f4.x, f4.y, f4.z, f4.w);
}

void ShaderProgram::setf9(const char* name, const mat3& f9) {
	glUniformMatrix3fv(glGetUniformLocation(handle, name), 1, GL_FALSE, &f9[0][0]);
}

void ShaderProgram::setf16(const char* name, const mat4& f16) {
	glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, GL_FALSE, &f16[0][0]);
}

ShaderProgramBuilder& ShaderProgramBuilder::vertex(const char* source) {
	building.shaders.push_back({ GL_VERTEX_SHADER, 0, source });
	return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::fragment(const char* source) {
	building.shaders.push_back({ GL_FRAGMENT_SHADER, 0, source });
	return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::compute(const char* source) {
	building.shaders.push_back({ GL_COMPUTE_SHADER, 0, source });
	return *this;
}

ShaderProgram ShaderProgramBuilder::build() {
	return ShaderProgram(building);
}