#include "ModelType.h"
#include "stdio.h"
#include "stdlib.h"

ModelType::ModelType(int id, GLenum primitive, GLuint gpu_program, std::initializer_list<VertexAttribute> attribs) {
	id_ = id;
	primitive_ = primitive;
	gpu_program_ = gpu_program;
	VertexAttribute position("position"); // the position attribute is default
	position.gpu_program(gpu_program_);
	attribs_.push_back(position);
	for(VertexAttribute attr : attribs) {
		attr.gpu_program(gpu_program_);
		attribs_.push_back(attr);
	}
}

ModelType::~ModelType() {
	printf("DEBUG: MODELTYPE DESTROYED\n");
}

void ModelType::set_strides(GLsizei stride) {
	for(unsigned int i = 0; i < attribs_.size(); i++)  {
		attribs_[i].stride(stride);
	}
}

void ModelType::enable_attribs() {
	for(VertexAttribute attr : attribs_) {
		glVertexAttribPointer(
			attr.location_in_shader(),
			attr.num_components(),
			attr.datatype(),
			attr.is_normalized(),
			attr.stride(),
			attr.offset());
		glEnableVertexAttribArray(attr.location_in_shader());
	}
}