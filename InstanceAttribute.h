#ifndef INSTANCEATTRIBUTE_H_
#define INSTANCEATTRIBUTE_H_

#include <GL/glew.h>
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
* The standard instance attribute is a model matrix.
* It is always available in vertex shaders as "in mat4 model".
*/
class InstanceAttribute {

protected:
	GLsizei bytes_;
	const GLvoid* pointer_;

	std::function<void(GLuint,GLsizei,GLsizei)> enable_func_;

public:
	InstanceAttribute();
	InstanceAttribute(std::function<void(GLuint,GLsizei,GLsizei)> enable_func);
	~InstanceAttribute();

	void enable(GLuint gpu_program, GLsizei offset, GLsizei stride);

	void bytes(GLsizei bytes);

	GLsizei bytes();

	const GLvoid* pointer();
};

#endif