#pragma once

#include "lith/math.h"
#include "lith/typedef.h"

#include <vector>

struct ShaderSource {
	GLenum type;
	GLint handle;
	const char* source; // does not own
};

struct ShaderProgramData {
	std::vector<ShaderSource> shaders;
};

class ShaderProgram {
public:
	ShaderProgram();
	ShaderProgram(const ShaderProgramData& data);

	ShaderProgram& compile();
	void free();

	// Return true if the shader program had successfully compiled
	bool use();

	void dispatch(int x, int y, int z);

	void seti(const char* name, int i1);
    void setf(const char* name, float f1);
	void setf2(const char* name, const vec2& f2);
	void setf3(const char* name, const vec3& f3);
	void setf4(const char* name, const vec4& f4);
	void setf9(const char* name, const mat3& f9);
	void setf16(const char* name, const mat4& f16);

private:
	GLuint handle;
	ShaderProgramData data;

	bool sucessfullyCompiled;
};

class ShaderProgramBuilder {
public:
	// Load the shader from a source file using 
	// a simple parser
	//ShaderProgram& source(const char* sourceFile);

	ShaderProgramBuilder& vertex(const char* source);
	ShaderProgramBuilder& fragment(const char* source);
	ShaderProgramBuilder& compute(const char* source);

	ShaderProgram build();

private:
	ShaderProgramData building;
};