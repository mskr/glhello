#ifndef INSTANCEATTRIBUTE_H_
#define INSTANCEATTRIBUTE_H_

#include <GL/glew.h>
#include <glm.hpp>
#include <stdexcept>
#include "stdio.h"
#include <functional>

/*
* This is an model instance attribute.
* People also call it instanced arrays.
* They are basically like vertex attributes.
* Except they are not updated for each vertex but for each instance.
* This framework always does instanced draw.
* ModelTypes can have prototype InstanceAttributes.
* The prototype InstanceAttributes are static members of classes that want to provide per-instance shader data.
* These classes derive from this class.
* Models must have the InstanceAttributes dictated by their ModelType in the same order.
* All instances have the same number of attributes but can have different values.
* The standard instance attribute is a model matrix.
* It is always available in vertex shaders as "in mat4 model".
*/
class InstanceAttribute {

protected:
	GLsizei bytes_;
	const GLvoid* pointer_;
	std::function<void(GLuint,GLsizei,GLsizei)> enable_func_;
	bool has_changed_;

public:
	InstanceAttribute();
	InstanceAttribute(glm::mat4* model_matrix_ptr);
	// Constructor for static instance attributes that modeltypes use
	InstanceAttribute(std::function<void(GLuint,GLsizei,GLsizei)> enable_func);
	~InstanceAttribute();

	void enable(GLuint gpu_program, GLsizei offset, GLsizei stride);

	void update(GLintptr offset);

	void bytes(GLsizei bytes);
	void pointer(GLvoid* pointer);

	GLsizei bytes();
	const GLvoid* pointer();
	bool has_changed() { return has_changed_; }
	// Used for model matrices. Other instance attribs can avoid this because they are subclasses.
	void force_change();
};

#endif