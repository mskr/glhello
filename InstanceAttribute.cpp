#include "InstanceAttribute.h"

InstanceAttribute::InstanceAttribute() {
	// assume bytes of a 4x4 matrix by default
	bytes_ = 4*4 * sizeof(GLfloat);
	pointer_ = 0;
	enable_func_ = 0;
	has_changed_ = false; // not used by model matrices
}


InstanceAttribute::InstanceAttribute(std::function<void(GLuint,GLsizei,GLsizei)> enable_func) {
	bytes_ = -1;
	pointer_ = 0;
	enable_func_ = enable_func;
	has_changed_ = false;
}

InstanceAttribute::~InstanceAttribute() {

}

void InstanceAttribute::enable(GLuint gpu_program, GLsizei offset, GLsizei stride) {
	if(enable_func_ != 0) {
		enable_func_(gpu_program, offset, stride);
		return;
	}
	// Enable the model matrix when this method is not overidden
	GLsizei column_bytes = 4 * sizeof(GLfloat);
	GLuint loc = glGetAttribLocation(gpu_program, "model");
	for(int count = 0; count < 4; count++, loc++) {
		glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, 
			(GLvoid*) (offset + count * column_bytes)
		);
		glVertexAttribDivisor(loc, 1);
		glEnableVertexAttribArray(loc);
	}
}

void InstanceAttribute::bytes(GLsizei bytes) {
	bytes_ = bytes;
}

void InstanceAttribute::was_updated() {
	has_changed_ = false;
}

GLsizei InstanceAttribute::bytes() {
	if(bytes_ == -1)
		throw std::runtime_error("Program exits because a model instance attribute is incorrect (bytes=-1).");
	return bytes_;
}

const GLvoid* InstanceAttribute::pointer() {
	if(pointer_ == 0)
		throw std::runtime_error("Program exits because a model instance attribute is incorrect (pointer=0).");
	return pointer_;
}