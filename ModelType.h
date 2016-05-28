#ifndef MODELTYPE_H_
#define MODELTYPE_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>

#include <vector>

#include "VertexAttribute.h"

/*
* This is a model-type.
* It is used to group similar models.
* This helps to save OpenGL state changes.
* You can give this to a model upon construction.
*/
class ModelType {

	int id_;
	GLenum primitive_;
	GLuint gpu_program_;
	std::vector<VertexAttribute> attribs_;

public:

	ModelType(int id, GLenum primitive, GLuint gpu_program, std::initializer_list<VertexAttribute> attribs);
	~ModelType();

	void enable_attribs();

	// GETTER
	GLenum primitive() { return primitive_; }
	GLuint gpu_program() { return gpu_program_; }
	VertexAttribute* vertex_attr(int pos) { return &(attribs_[pos]); }
	int num_vertex_attribs() { return attribs_.size(); }
	int id() { return id_; }

	// SETTER
	void set_strides(GLsizei stride);

};


#endif