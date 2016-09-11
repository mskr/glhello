#include "ModelType.h"

ModelType::ModelType(int id, GLenum primitive, GLuint gpu_program,
  std::initializer_list<VertexAttribute> vertex_attribs,
  std::initializer_list<InstanceAttribute> instance_attribs) {
	id_ = id;
	primitive_ = primitive;
	gpu_program_ = gpu_program;
	// "vec3 position" is an implicit vertex attribute
	VertexAttribute position;
	attribs_.push_back(position);
	for(VertexAttribute attr : vertex_attribs)
		attribs_.push_back(attr);
	// "mat4 model" is an implicit instance attribute
	instance_attribs_ = {InstanceAttribute()};
	instance_attribs_.insert(instance_attribs_.end(), instance_attribs.begin(), instance_attribs.end());
	bytes_instance_attribs_ = 0;
	for(InstanceAttribute attr : instance_attribs_) {
		bytes_instance_attribs_ += attr.bytes();
	}
}

ModelType::~ModelType() {
	printf("DEBUG: ModelType %d destroyed.\n", id_);
}

void ModelType::set_strides(GLsizei stride) { // used in Model::vertices()
	for(unsigned int i = 0; i < attribs_.size(); i++) {
		attribs_[i].stride(stride);
	}
}

void ModelType::bind_vertex_attr_location(int attr_index, GLint location) {
	attribs_[attr_index].bind_location(gpu_program_, location);
}


int ModelType::bind_instance_attr_locations(int attr_index, GLint start_location) {
	return instance_attribs_[attr_index].bind_locations(gpu_program_, start_location);
}

void ModelType::link_gpu_program() {
	glLinkProgram(gpu_program_);
}

void ModelType::format_vertex_buffer() { // used in World constructor
	for(VertexAttribute attr : attribs_) {
		if(!attr.test_used_by(gpu_program_))
			printf("WARNING: ModelType \"%d\" has vertex attribute \"%s\" that is not used in its shader.\n", id_, attr.name());
		attr.format();
	}
}

void ModelType::format_instance_buffer() {
	GLsizei offset = 0;
	for(InstanceAttribute &attr : instance_attribs_) {
		attr.format(offset, bytes_instance_attribs_);
		offset += attr.bytes();
	}
}