#ifndef VERTEXATTRIBUTE_H_
#define VERTEXATTRIBUTE_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include <stdexcept>
#include "stdio.h"

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
	GLint location_;

public:
	VertexAttribute(); // creates a position attribute
	VertexAttribute(const GLchar* name);
	~VertexAttribute();

	bool equals(VertexAttribute* other);

	bool test_used_by(GLuint gpu_program);

	void bind_location(GLuint gpu_program, GLint location);

	void format();

	// GETTER
	GLint num_components() { return num_components_; }
	GLenum datatype() { return datatype_; }
	GLboolean is_normalized() { return is_normalized_; }
	GLsizei stride() { return stride_; }
	const GLvoid* offset() { return offset_; }
	const GLchar* name() { return name_; }
	GLint location() { return location_; }

	// SETTER
	// Must be called *before* format()
	void num_components(GLint num_components); // set by Model::vertices()
	void stride(GLsizei s); // set by Model::vertices()
	void offset(const GLvoid* o); // set by Model::vertices()

};


#endif