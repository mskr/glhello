#include "InstanceAttribute.h"

InstanceAttribute::InstanceAttribute() {
	bytes_ = -1;
	pointer_ = 0;
	enable_func_ = 0;
	has_changed_ = false;
}

InstanceAttribute::InstanceAttribute(glm::mat4* model_matrix_ptr) {
	// assume bytes of a 4x4 matrix by default
	bytes_ = 4*4 * sizeof(GLfloat);
	pointer_ = model_matrix_ptr;
	enable_func_ = 0;
	has_changed_ = false;
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
	printf("enable matrix\n");
	// Enable the model matrix when this method is not overidden
	const GLchar* varname = "model";
	GLsizei column_bytes = 4 * sizeof(GLfloat);
	GLint loc = glGetAttribLocation(gpu_program, varname);
	if(loc == -1) printf("WARNING: Attribute \"%s\" not found in shader.\n", varname);
	for(int count = 0; count < 4; count++, loc++) {
		glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, stride, 
			(GLvoid*) (offset + count * column_bytes)
		);
		glVertexAttribDivisor(loc, 1);
		glEnableVertexAttribArray(loc);
	}
}

void InstanceAttribute::update(GLintptr offset) {
	if(!has_changed_) return;
	glBufferSubData(GL_ARRAY_BUFFER, offset, bytes_, pointer_);
	has_changed_ = false;
}

void InstanceAttribute::force_change() {
	has_changed_ = true;
}

void InstanceAttribute::bytes(GLsizei bytes) { bytes_ = bytes; }
void InstanceAttribute::pointer(GLvoid* pointer) { pointer_ = pointer; }

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