#ifndef INSTANCEATTRIBUTE_H_
#define INSTANCEATTRIBUTE_H_

#include <GL/glew.h>
#include <stdexcept>
#include "stdio.h"
#include <functional>

/*
* This is a model instance attribute.
* An instance attribute is per-instance shader data.
* It is used with hardware instancing.
* (People also talk about instanced arrays.)
* Instances of a model have the same attributes.
* These are taken from modeltype.
* Each instance can can be set to different attribute values.
* Instance attributes are technically like vertex attributes.
* Except they are not updated for each vertex but for each instance.
* If not subclassed, this class enables "in mat4 model" in vertex shader.
* To define a new instance attribute, subclass this.
*/
class InstanceAttribute {

protected:
	GLsizei bytes_;
	const GLvoid* pointer_;

	// Indicates if gpu buffer has to be updated
	bool has_changed_;

	// String describing the memory layout of the attribute components.
	// Has the form "Name1: ffff, Name2: iiii.".
	// Character 'f' stands for one float, 'i' for one int.
	// "NameX" is a variable name in shader.
	std::string memory_layout_;

public:
	// 1st constructor called only internally by modeltype to create the implicit model matrix instance attribute
	InstanceAttribute();
	// 2nd constructor called by superclasses' default constructors (use with modeltypes)
	InstanceAttribute(GLsizei bytes, const GLvoid* pointer, std::string memory_layout);
	// 3rd constructor called by superclasses' non-default constructors (use with models and instances)
	InstanceAttribute(GLsizei bytes, const GLvoid* pointer);
	~InstanceAttribute();

	void enable(GLuint gpu_program, GLsizei offset, GLsizei stride);
	
	void was_updated(); // for runtime updates

	GLsizei bytes() { return bytes_; };
	const GLvoid* pointer() { return pointer_; };
	bool has_changed() { return has_changed_; }
};

#endif