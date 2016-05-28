#include "VertexAttribute.h"

#include "stdio.h"
#include "stdlib.h"

VertexAttribute::VertexAttribute(const GLchar* name) {
	name_ = name;
	num_components_ = 0;
	datatype_ = GL_FLOAT;
	is_normalized_ = GL_FALSE;
}

VertexAttribute::VertexAttribute(const GLchar* name, GLint num_components) {
	name_ = name;
	num_components_ = num_components;
	datatype_ = GL_FLOAT;
	is_normalized_ = GL_FALSE;
}

VertexAttribute::VertexAttribute(const GLchar* name, GLint num_components, GLenum datatype) {
	name_ = name;
	num_components_ = num_components;
	datatype_ = datatype;
	is_normalized_ = GL_FALSE;
}

VertexAttribute::~VertexAttribute() {
	
}

// GETTER
GLuint VertexAttribute::location_in_shader() {
	if(gpu_program_ == 0)
		throw std::runtime_error("Program exits because vertex attribute of non-existing shader requested.");
	GLint location = glGetAttribLocation(gpu_program_, name_);
	if(location == -1) {
		printf("ERROR: Vertex attribute \"%s\" not found in one shader (not defined or never used).\n", name_);
		exit(EXIT_FAILURE);
	}
	return location;
}

// SETTER
void VertexAttribute::num_components(GLint num_components) { num_components_ = num_components; }
void VertexAttribute::gpu_program(GLuint program_id) { gpu_program_ = program_id; }
void VertexAttribute::stride(GLsizei s) { stride_ = s; }
void VertexAttribute::offset(const GLvoid* o) { offset_ = o; }