#include "ModelType.h"

ModelType::ModelType(int id, GLenum primitive, GLuint gpu_program, std::initializer_list<VertexAttribute> attribs) {
	id_ = id;
	primitive_ = primitive;
	gpu_program_ = gpu_program;
	VertexAttribute position("position"); // "attribute vec3 position" available in shader by default
	position.gpu_program(gpu_program_);
	attribs_.push_back(position);
	for(VertexAttribute attr : attribs) {
		attr.gpu_program(gpu_program_);
		attribs_.push_back(attr);
	}
	instance_attribs_ = {InstanceAttribute()}; // "attribute mat4 model" available in shader by default
	bytes_instance_attribs_ = -1;
}

ModelType::~ModelType() {
	printf("DEBUG: MODELTYPE DESTROYED\n");
}

void ModelType::instance_attribs(std::initializer_list<InstanceAttribute> attribs) {
	for(InstanceAttribute attr : attribs) {
		instance_attribs_.push_back(attr);
	}
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

void ModelType::enable_instance_attribs() {
	GLsizei stride = 0;
	for(InstanceAttribute &attr : instance_attribs_) {
		stride += attr.bytes();
	}
	GLsizei offset = 0;
	for(InstanceAttribute &attr : instance_attribs_) {
		attr.enable(gpu_program_, offset, stride);
		offset += attr.bytes();
	}
	bytes_instance_attribs_ = stride;
}