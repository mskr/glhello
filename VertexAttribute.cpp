#include "VertexAttribute.h"

VertexAttribute::VertexAttribute() {
	name_ = "position";
	num_components_ = 0;
	stride_ = 0;
	offset_ = 0;
	location_ = 0;
	datatype_ = GL_FLOAT;
	is_normalized_ = GL_FALSE;
}

VertexAttribute::VertexAttribute(const GLchar* name) {
	name_ = name;
	num_components_ = 0;
	stride_ = 0;
	offset_ = 0;
	location_ = 0;
	datatype_ = GL_FLOAT;
	is_normalized_ = GL_FALSE;
}

// VertexAttribute::VertexAttribute(const GLchar* name, GLint num_components) {
// 	name_ = name;
// 	num_components_ = num_components;
// 	stride_ = 0;
// 	offset_ = 0;
// 	location_ = 0;
// 	datatype_ = GL_FLOAT;
// 	is_normalized_ = GL_FALSE;
// }

// VertexAttribute::VertexAttribute(const GLchar* name, GLint num_components, GLenum datatype) {
// 	name_ = name;
// 	num_components_ = num_components;
// 	stride_ = 0;
// 	offset_ = 0;
// 	location_ = 0;
// 	datatype_ = datatype;
// 	is_normalized_ = GL_FALSE;
// }

VertexAttribute::~VertexAttribute() {
	
}

bool VertexAttribute::equals(VertexAttribute* other) {
	if(other == 0) return false;
	return (std::string(this->name_).compare(other->name()) == 0);
}

bool VertexAttribute::test_used_by(GLuint gpu_program) {
	return (glGetAttribLocation(gpu_program, name_) != -1);
}

void VertexAttribute::bind_location(GLuint gpu_program, GLint location) {
	glBindAttribLocation(gpu_program, location, name_);
	location_ = location;
}

void VertexAttribute::format() {
	glVertexAttribPointer(location_, num_components_, datatype_, is_normalized_, stride_, offset_);
	glEnableVertexAttribArray(location_);
}

// SETTER
void VertexAttribute::num_components(GLint num_components) { num_components_ = num_components; }
void VertexAttribute::stride(GLsizei s) { stride_ = s; }
void VertexAttribute::offset(const GLvoid* o) { offset_ = o; }