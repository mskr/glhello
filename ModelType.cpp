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
	instance_attribs_ = {InstanceAttribute(0)}; // "attribute mat4 model" available in shader by default
	bytes_instance_attribs_ = 0;
}

ModelType::~ModelType() {
	printf("DEBUG: MODELTYPE DESTROYED\n");
}

void ModelType::instance_attribs(std::initializer_list<InstanceAttribute> attribs) {
	for(InstanceAttribute attr : attribs) {
		instance_attribs_.push_back(attr);
	}
}

void ModelType::enable_attribs() {
	GLsizei bytes = 0;
	for(VertexAttribute &attr : attribs_)
		bytes += attr.num_components() * sizeof(GLfloat);
	for(VertexAttribute &attr : attribs_)
		attr.stride(bytes);
	for(VertexAttribute &attr : attribs_) {
		printf("enable modeltype%d vert attrib: program=%d, offset=%d, stride=%d\n", id_, gpu_program_, attr.offset(), (int)attr.stride());
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
	for(InstanceAttribute &attr : instance_attribs_)
		bytes_instance_attribs_ += attr.bytes();
	printf("instance attribs overall bytes=%d\n", bytes_instance_attribs_);
	GLsizei offset = 0;
	for(InstanceAttribute &attr : instance_attribs_) {
		printf("enable modeltype%d inst attrib: program=%d, offset=%d, stride=%d\n", id_, gpu_program_, offset, bytes_instance_attribs_);
		attr.enable(gpu_program_, offset, bytes_instance_attribs_);
		offset += attr.bytes();
	}
}