#ifndef VERTEXATTRIBUTE_H_
#define VERTEXATTRIBUTE_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <stdexcept>

/*
* This is a vertex attribute.
* A vertex can have multiple attributes.
* A attribute can have multiple components.
* A component has a data type (here: just floats!).
* Attribute values are available in vertex shader.
*/
class VertexAttribute {

	const GLchar* name_;
	GLint num_components_;
	GLenum datatype_;
	GLboolean is_normalized_;
	GLsizei stride_;
	const GLvoid* offset_;
	GLuint gpu_program_;

public:
	VertexAttribute(const GLchar* name);
	VertexAttribute(const GLchar* name, GLint num_components);
	VertexAttribute(const GLchar* name, GLint num_components, GLenum datatype);
	~VertexAttribute();

	// GETTER
	GLuint location_in_shader();
	GLint num_components() { return num_components_; }
	GLenum datatype() { return datatype_; }
	GLboolean is_normalized() { return is_normalized_; }
	GLsizei stride() { return stride_; }
	const GLvoid* offset() { return offset_; }
	const GLchar* name() { return name_; }

	// SETTER
	void num_components(GLint num_components);
	void gpu_program(GLuint program_id);
	void stride(GLsizei s);
	void offset(const GLvoid* o);

};


#endif