#include "ModelInstance.h"
#include "Model.h"
#include "stdio.h"

ModelInstance::ModelInstance(int id, int instance_id, Model* instance_of) {
	id_ = id;
	instance_id_ = instance_id;
	position_ = glm::vec3(0.0f);
	instance_of_ = instance_of;
	has_changed_ = false;
	units_x_ = -1;
	units_y_ = -1;
	units_z_ = -1;
	emitter_ = 0;
}

ModelInstance::~ModelInstance() {
	printf("DEBUG: MODEL INSTANCE DESTROYED (model id=%d, instance id=%d)\n", id_, instance_id_);
}





ModelInstance* ModelInstance::attr(unsigned int index, InstanceAttribute attrib) {
	if(index == 0) { // Prevent access to model matrix
		printf("WARNING: Model %d's instance %d's attribute 0 cannot be accessed.\n", id_, instance_id_);
		return this;
	} else if(index < 0 || index >= attribs_.size()) { // Prevent out of bounds access
		printf("WARNING: Model %d's instance %d's attribute %d out of bounds.\n", id_, instance_id_, index);
		return this;
	} else if(attrib.bytes() != attribs_[index].bytes()) { // Prevent different size (which would corrupt the gpu buffer)
		printf("WARNING: Model %d's instance %d's attribute %d could not be set because of wrong size.\n", id_, instance_id_, index);
		return this;
	}
	attribs_[index] = attrib;
	return this;
}

void ModelInstance::emit(unsigned int index, Light::Emitter emitter) {
	attr(index, emitter);
	emitter.update_position(position_);
	emitter_ = &emitter;
}

ModelInstance* ModelInstance::sub(std::initializer_list<ModelInstance*> children) {
	for(ModelInstance* child : children) {
		if(child != this)
			sub_.push_back(child);
	}
	return this;
}





glm::mat4* ModelInstance::model_matrix() {
	return instance_of_->matrix_at(instance_id_);
}

void ModelInstance::transform(glm::mat4 transformation_matrix) {
	glm::mat4* ptr = model_matrix();
	*ptr = transformation_matrix * (*ptr);
	for(ModelInstance* child : sub_)
		child->transform(transformation_matrix);
	has_changed_ = true;
	if(emitter_ != 0)
		emitter_->update_position(position_);
}

void ModelInstance::was_updated() {
	has_changed_ = false;
}




// === TRANSFORM SHORTHAND FUNCTIONS ===

ModelInstance* ModelInstance::units(float x_units, float y_units, float z_units) {
	if(units_x_ > 0.0f) scaleX(x_units/units_x_);
	if(units_y_ > 0.0f) scaleY(y_units/units_y_);
	if(units_z_ > 0.0f) scaleZ(z_units/units_z_);
	return this;
}

ModelInstance* ModelInstance::units(glm::vec3 units) {
	unitsX(units.x);
	unitsY(units.y);
	unitsZ(units.z);
	return this;
}

ModelInstance* ModelInstance::unitsX(float units) {
	if(units_x_ <= 0.0f) {
		printf("WARNING: Scaling model %d to %f units x-length aborted ", id_, units);
		(units_x_ == 0.0f) ? printf("(x-length is zero).\n") : printf("(x-length not initialized).\n");
		return this;
	}
	scale(units/units_x_);
	return this;
}

ModelInstance* ModelInstance::unitsY(float units) {
	if(units_y_ <= 0.0f) {
		printf("WARNING: Scaling model %d to %f units y-length aborted ", id_, units);
		(units_y_ == 0.0f) ? printf("(y-length is zero).\n") : printf("(y-length not initialized).\n");
		return this;
	}
	scale(units/units_y_);
	return this;
}

ModelInstance* ModelInstance::unitsZ(float units) {
	if(units_z_ <= 0.0f) {
		printf("WARNING: Scaling model %d to %f units y-length aborted. ", id_, units);
		(units_z_ == 0.0f) ? printf("(z-length is zero).\n") : printf("(z-length not initialized).\n");
		return this;
	}
	scale(units/units_z_);
	return this;
}

ModelInstance* ModelInstance::rotate(float degrees, glm::vec3 axis, glm::vec3 transform_origin) {
	glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -transform_origin);
	glm::mat4 r = glm::rotate(glm::radians(degrees), axis);
	glm::mat4 t2 = glm::translate(glm::mat4(1.0f), transform_origin);
	glm::mat4 m = t2 * r * t1;
	glm::vec4 v = m * glm::vec4(position_,1);
	position_ = glm::vec3(v.x,v.y,v.z);
	for(ModelInstance* child : sub_) { //TODO Is transform_origin really the same for children?
		glm::vec4 v = m * glm::vec4(child->position_,1);
		child->position_ = glm::vec3(v.x,v.y,v.z);
	}
	transform(m);
	return this;
}

ModelInstance* ModelInstance::rotate(float degrees, glm::vec3 axis) {
	glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -position_);
	glm::mat4 r = glm::rotate(glm::radians(degrees), axis);
	glm::mat4 t2 = glm::translate(glm::mat4(1.0f), position_);
	glm::mat4 m = t2 * r * t1;
	for(ModelInstance* child : sub_) {
		glm::vec4 v = m * glm::vec4(child->position_,1);
		child->position_ = glm::vec3(v.x,v.y,v.z);
	}
	transform(m);
	return this;
}

ModelInstance* ModelInstance::rotateX(float degrees) {
	rotate(degrees, glm::vec3(1.0f, 0.0f, 0.0f));
	return this;
}

ModelInstance* ModelInstance::rotateY(float degrees) {
	rotate(degrees, glm::vec3(0.0f, 1.0f, 0.0f));
	return this;
}

ModelInstance* ModelInstance::rotateZ(float degrees) {
	rotate(degrees, glm::vec3(0.0f, 0.0f, 1.0f));
	return this;
}

ModelInstance* ModelInstance::translate(glm::vec3 distances_in_units) {
	glm::vec3 distances_in_modelspace(
		distances_in_units.x * config::one_unit_x, 
		distances_in_units.y * config::one_unit_y,
		distances_in_units.z * config::one_unit_z);
	position_ += distances_in_modelspace;
	for(ModelInstance* child : sub_)
		child->position_ += distances_in_modelspace;
	transform(glm::translate(glm::mat4(1.0f), distances_in_modelspace));
	return this;
}

ModelInstance* ModelInstance::translate(float x, float y, float z) {
	translate(glm::vec3(x,y,z));
	return this;
}

ModelInstance* ModelInstance::translateX(float distance_in_units) {
	glm::vec3 distance_in_modelspace(distance_in_units*config::one_unit_x, 0.0f, 0.0f);
	position_.x += distance_in_modelspace.x;
	for(ModelInstance* child : sub_)
		child->position_.x += distance_in_modelspace.x;
	transform(glm::translate(glm::mat4(1.0f), distance_in_modelspace));
	return this;
}

ModelInstance* ModelInstance::translateY(float distance_in_units) {
	glm::vec3 distance_in_modelspace(0.0f, distance_in_units*config::one_unit_y, 0.0f);
	position_.y += distance_in_modelspace.y;
	for(ModelInstance* child : sub_)
		child->position_.y += distance_in_modelspace.y;
	transform(glm::translate(glm::mat4(1.0f), distance_in_modelspace));
	return this;
}

ModelInstance* ModelInstance::translateZ(float distance_in_units) {
	glm::vec3 distance_in_modelspace(0.0f, 0.0f, distance_in_units*config::one_unit_z);
	position_.z += distance_in_modelspace.z;
	for(ModelInstance* child : sub_)
		child->position_.z += distance_in_modelspace.z;
	transform(glm::translate(glm::mat4(1.0f), distance_in_modelspace));
	return this;
}

ModelInstance* ModelInstance::scale(float factor) {
	if(factor == 0.0f) {
		// we do not let the model be scaled to zero because we would never be able to scale it back again
		printf("WARNING: Scaling model %d aborted. Please do not scale a model to zero.\n", id_);
		return this;
	}
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(-position_.x, -position_.y, -position_.z)));
	// transform(glm::scale(glm::mat4(1.0f), glm::vec3(factor)));
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(position_.x, position_.y, position_.z)));
	units_x_ *= factor;
	units_y_ *= factor;
	units_z_ *= factor;
	glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -position_);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(factor));
	glm::mat4 t2 = glm::translate(glm::mat4(1.0f), position_);
	glm::mat4 m = t2 * s * t1;
	for(ModelInstance* child : sub_) {
		child->units_x_ *= factor;
		child->units_y_ *= factor;
		child->units_z_ *= factor;
		child->position_ *= factor;
	}
	transform(m);
	return this;
}

ModelInstance* ModelInstance::scaleX(float factor) {
	if(factor == 0.0f) {
		printf("WARNING: Scaling model %d aborted. Please do not scale a model to zero.\n", id_);
		return this;
	}
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(-position_.x, -position_.y, -position_.z)));
	// transform(glm::scale(glm::mat4(1.0f), glm::vec3(factor, 1.0f, 1.0f)));
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(position_.x, position_.y, position_.z)));
	units_x_ *= factor;
	glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -position_);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(factor, 1.0f, 1.0f));
	glm::mat4 t2 = glm::translate(glm::mat4(1.0f), position_);
	glm::mat4 m = t2 * s * t1;
	for(ModelInstance* child : sub_) {
		child->units_x_ *= factor;
		child->position_.x *= factor;
	}
	transform(m);
	return this;
}

ModelInstance* ModelInstance::scaleY(float factor) {
	if(factor == 0.0f) {
		printf("WARNING: Scaling model %d aborted. Please do not scale a model to zero.\n", id_);
		return this;
	}
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(-position_.x, -position_.y, -position_.z)));
	// transform(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, factor, 1.0f)));
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(position_.x, position_.y, position_.z)));
	units_y_ *= factor;
	glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -position_);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, factor, 1.0f));
	glm::mat4 t2 = glm::translate(glm::mat4(1.0f), position_);
	glm::mat4 m = t2 * s * t1;
	for(ModelInstance* child : sub_) {
		child->units_y_ *= factor;
		child->position_.y *= factor;
	}
	transform(m);
	return this;
}

ModelInstance* ModelInstance::scaleZ(float factor) {
	if(factor == 0.0f) {
		printf("WARNING: Scaling model %d aborted. Please do not scale a model to zero.\n", id_);
		return this;
	}
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(-position_.x, -position_.y, -position_.z)));
	// transform(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, factor)));
	// transform(glm::translate(glm::mat4(1.0f), glm::vec3(position_.x, position_.y, position_.z)));
	units_z_ *= factor;
	glm::mat4 t1 = glm::translate(glm::mat4(1.0f), -position_);
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, factor));
	glm::mat4 t2 = glm::translate(glm::mat4(1.0f), position_);
	glm::mat4 m = t2 * s * t1;
	for(ModelInstance* child : sub_) {
		child->units_z_ *= factor;
		child->position_.z *= factor;
	}
	transform(m);
	return this;
}

ModelInstance* ModelInstance::position(glm::vec3 pos_in_units_from_origin) {
	translate(pos_in_units_from_origin - position());
	return this;
}

ModelInstance* ModelInstance::position(float x, float y, float z) {
	return position(glm::vec3(x,y,z));
}