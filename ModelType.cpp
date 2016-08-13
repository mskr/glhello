#include "ModelType.h"

ModelType::ModelType(int id, GLenum primitive, GLuint gpu_program,
  std::initializer_list<VertexAttribute> vertex_attribs,
  std::initializer_list<InstanceAttribute> instance_attribs) {
	id_ = id;
	primitive_ = primitive;
	gpu_program_ = gpu_program;
	// "attribute vec3 position" is an implicit vertex attribute
	VertexAttribute position("position");
	position.gpu_program(gpu_program_);
	attribs_.push_back(position);
	for(VertexAttribute attr : vertex_attribs) {
		attr.gpu_program(gpu_program_);
		attribs_.push_back(attr);
	}
	// "attribute mat4 model" is an implicit instance attribute
	instance_attribs_ = {InstanceAttribute()};
	instance_attribs_.insert(instance_attribs_.end(), instance_attribs.begin(), instance_attribs.end());
	bytes_instance_attribs_ = 0;
	for(InstanceAttribute attr : instance_attribs_) {
		bytes_instance_attribs_ += attr.bytes();
	}
}

ModelType::~ModelType() {
	printf("DEBUG: MODELTYPE DESTROYED\n");
}

void ModelType::set_strides(GLsizei stride) {
	for(unsigned int i = 0; i < attribs_.size(); i++) {
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

void ModelType::enable_instance_attribs() {
	GLsizei offset = 0;
	for(InstanceAttribute &attr : instance_attribs_) {
		attr.enable(gpu_program_, offset, bytes_instance_attribs_);
		offset += attr.bytes();
	}
}