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
* WARNING:
* Equal instance attributes of models of one type must have equal number of bytes.
*/
class InstanceAttribute {

protected:
	GLsizei bytes_;
	const GLvoid* pointer_;
	std::function<void(GLuint,GLsizei,GLsizei)> enable_func_;
	bool has_changed_;

	// Holds the index under which the attr can be accessed through model instances
	unsigned int index_;
	std::function<void(unsigned int)> index_func_;

public:
	InstanceAttribute();
	InstanceAttribute(std::function<void(GLuint,GLsizei,GLsizei)> enable_func);
	~InstanceAttribute();

	void enable(GLuint gpu_program, GLsizei offset, GLsizei stride);
	void bytes(GLsizei bytes);
	void nullpointer();
	void was_updated();

	GLsizei bytes();
	const GLvoid* pointer();
	bool has_changed() { return has_changed_; }

	// Used by models to store the index under which it can be accessed through a model instance
	void call_index_func(unsigned int i);
	void set_index(unsigned int i);
	unsigned int index() { return index_; }
};

#endif