#include "Model.h"

Model::Model(int id, ModelType* modeltype, std::initializer_list<InstanceAttribute> instance_attribs) : ModelInstance(id, 0, this) {
	modeltype_ = modeltype;
	vertices_ = {};
	instances_ = {this};
	matrices_ = {glm::mat4(1.0f)};
	attribs_ = {InstanceAttribute()};
	int i = 1;
	for(InstanceAttribute attr : instance_attribs) {
		attribs_.push_back(attr);
		attr.call_index_func(attribs_.size()-1);
		modeltype_->instance_attr(i)->bytes(attr.bytes());
		i++;
	}
	if(attribs_.size() != modeltype_->num_instance_attribs())
		throw std::runtime_error("Program exits because number of instance attributes of model does not match its modeltype.");
	num_new_instances_ = 0;
}

Model::Model(int id, ModelType* modeltype, std::initializer_list<std::initializer_list<std::initializer_list<GLfloat>>> v,
  std::initializer_list<InstanceAttribute> instance_attribs) : ModelInstance(id, 0, this) {

	modeltype_ = modeltype;
	instances_ = {this};
	matrices_ = {glm::mat4(1.0f)};
	attribs_ = {InstanceAttribute()};
	int i = 1;
	for(InstanceAttribute attr : instance_attribs) {
		attribs_.push_back(attr);
		attr.call_index_func(attribs_.size()-1);
		modeltype_->instance_attr(i)->bytes(attr.bytes());
		i++;
	}
	if(attribs_.size() != modeltype_->num_instance_attribs())
		throw std::runtime_error("Program exits because number of instance attributes of model does not match its modeltype.");
	num_new_instances_ = 0;
	// convert initializer list to vector
	std::vector<std::vector<std::vector<GLfloat>>> v_vertices = {};
	for(std::initializer_list<std::initializer_list<GLfloat>> vertex : v) {
		std::vector<std::vector<GLfloat>> v_vertex = {};
		for(std::initializer_list<GLfloat> vertex_attr : vertex) {
			std::vector<GLfloat> v_vertex_attr(vertex_attr);
			v_vertex.push_back(v_vertex_attr);
		}
		v_vertices.push_back(v_vertex);
	}
	vertices(v_vertices);
}

Model::~Model() {
	for(GLfloat &el : vertices_) el = 0;
	for(ModelInstance* el : instances_) delete el;
	printf("DEBUG: MODEL (id=%d) DESTROYED\n", id_);
}

void Model::vertices(std::vector<std::vector<std::vector<GLfloat>>> vertices) {
	vertices_.clear();
	num_vertices_ = vertices.size();
	// test if number of vertex attributes given here equals the number given to modeltype_
	bool is_consistent_with_modeltype = true;
	float min_x = std::numeric_limits<float>::infinity();
	float max_x = -std::numeric_limits<float>::infinity();
	float min_y = std::numeric_limits<float>::infinity();
	float max_y = -std::numeric_limits<float>::infinity();
	float min_z = std::numeric_limits<float>::infinity();
	float max_z = -std::numeric_limits<float>::infinity();
	for(std::vector<std::vector<GLfloat>> &vertex : vertices) {
		unsigned int i = 0;
		int components_counter = 0;
		for(std::vector<GLfloat> &vertex_attr : vertex) {
			is_consistent_with_modeltype = (i < modeltype_->num_vertex_attribs());
			modeltype_->vertex_attr(i)->num_components(vertex_attr.size());
			modeltype_->vertex_attr(i)->offset((GLvoid*)(components_counter * sizeof(GLfloat)));
			vertices_.insert(vertices_.end(), vertex_attr.begin(), vertex_attr.end());
			if(i==0) { // (Here, vertex_attr is the position)
				if(vertex_attr.size() < 3) throw std::runtime_error("Program exits because vertex position vector has less than 3 components.");
				// Find min and max coordinates
				if(vertex_attr[0] < min_x) min_x = vertex_attr[0];
				if(vertex_attr[0] > max_x) max_x = vertex_attr[0];
				if(vertex_attr[1] < min_y) min_y = vertex_attr[1];
				if(vertex_attr[1] > max_y) max_y = vertex_attr[1];
				if(vertex_attr[2] < min_z) min_z = vertex_attr[2];
				if(vertex_attr[2] > max_z) max_z = vertex_attr[2];
			}
			i++;
			components_counter += vertex_attr.size();
		}
		if(is_consistent_with_modeltype) is_consistent_with_modeltype = (i == modeltype_->num_vertex_attribs());
		if(!is_consistent_with_modeltype) throw std::runtime_error("Program exits because number of vertex attributes of model does not match its modeltype.");
		i = 0;
	}
	units_x_ = (max_x - min_x)/config::one_unit_x;
	units_y_ = (max_y - min_y)/config::one_unit_y;
	units_z_ = (max_z - min_z)/config::one_unit_z;
	if(units_x_==0 && units_y_==0 && units_z_==0) position_ = glm::vec3(max_x, max_y, max_z);
	modeltype_->set_strides(this->bytes()/num_vertices_);
}

void Model::draw(GLint offset, GLuint instance_attribs_offset) {
	glDrawArraysInstancedBaseInstance(modeltype_->primitive(), offset, num_vertices_, num_instances(), instance_attribs_offset);
}



ModelInstance* Model::use() {
	ModelInstance* inst = new ModelInstance(id_, instances_.size(), this);
	inst->units_x_ = this->units_x_;
	inst->units_y_ = this->units_y_;
	inst->units_z_ = this->units_z_;
	instances_.push_back(inst);
	matrices_.push_back(matrices_[0]); // copy matrix of parent model
	inst->attribs_ = this->attribs_; // copy other attribs too
	num_new_instances_++;
	return inst;
}

void Model::instances_added() {
	num_new_instances_ = 0;
}

GLsizeiptr Model::bytes_instance_attribs() {
	GLsizeiptr bytes = 0;
	for(ModelInstance* inst : instances_)
		for(InstanceAttribute &attrib : inst->attribs_)
			bytes += attrib.bytes();
	return bytes;
}

void Model::update_instance_attribs(GPUBuffer* b, GLint offset) {
	GLint o = offset * modeltype_->bytes_instance_attribs();
	for(unsigned int i = 0; i < instances_.size(); i++) {
		// check if model matrix update necessary
		if(instances_[i]->has_changed()) {
			b->bind();
			glBufferSubData(GL_ARRAY_BUFFER, o, sizeof(glm::mat4), &matrices_[i]);
			instances_[i]->was_updated();
		}
		//TODO check for other instance attributes (instance attr index starts at 1 !)
		o += modeltype_->bytes_instance_attribs();
	}
}

const GLvoid* Model::pointer_instance_attr(unsigned int instance_index, unsigned int attr_index) {
	if(instance_index >= instances_.size() || attr_index >= instances_[instance_index]->attribs_.size())
		throw std::runtime_error("Program exits because model instance attribute that was requested is out of range.");
	else if(attr_index == 0) return matrix_at(instance_index);
	else return instances_[instance_index]->attr(attr_index)->pointer();
}