#ifndef MODELTYPE_H_
#define MODELTYPE_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>

#include <vector>
#include "stdio.h"
#include "stdlib.h"

#include "VertexAttribute.h"
#include "InstanceAttribute.h"

/*
* This is a model-type.
* It is used to group similar models.
* Similar models
* a) consist of the same primitive (triangle, point, ...).
* b) are rendered with the same shader.
* c) have the same vertex attributes
* d) have the same instance attributes
* This helps to save OpenGL state changes.
* Models expect a type on construction.
*/
class ModelType {

	int id_;
	GLenum primitive_;
	GLuint gpu_program_;
	std::vector<VertexAttribute> attribs_;
	std::vector<InstanceAttribute> instance_attribs_;
	GLint bytes_instance_attribs_;

public:

	ModelType(int id, GLenum primitive, GLuint gpu_program,
		std::initializer_list<VertexAttribute> vertex_attribs,
		std::initializer_list<InstanceAttribute> instance_attribs);
	~ModelType();

	// Attribute location connects gpu program to buffer
	// World ensures that equally named attributes have equal location, across shaders
	void bind_vertex_attr_location(int attr_index, GLint location);
	int bind_instance_attr_locations(int attr_index, GLint location);

	void link_gpu_program();

	void format_vertex_buffer();
	void format_instance_buffer();

	// GETTER
	int id() { return id_; }
	GLenum primitive() { return primitive_; }
	GLuint gpu_program() { return gpu_program_; }
	VertexAttribute* vertex_attr(int pos) { return &(attribs_[pos]); }
	unsigned int num_vertex_attribs() { return attribs_.size(); }
	InstanceAttribute* instance_attr(int pos) { return &(instance_attribs_[pos]); }
	unsigned int num_instance_attribs() { return instance_attribs_.size(); }
	GLint bytes_instance_attribs() { return bytes_instance_attribs_; }

	// SETTER
	void set_strides(GLsizei stride);

};


#endif