#ifndef MODEL_INSTANCE_H_
#define MODEL_INSTANCE_H_

#include <glm.hpp>
#include <gtx/transform.hpp>

#include "config.h"
#include "Material.h"

class Model;

/*
* This is an instance of a model.
* It is used with the OpenGL instanced draw feature.
* It has always the same vertex data as the model.
* But it can have a different model matrix.
* This means you can efficiently render the same shape to different positions.
*/
class ModelInstance {

	// Index of this instance (equals gl_InstanceID in shader)
	int instance_id_;

	// Pointer to the (parent)-model that this is an instance of
	Model* instance_of_;

	// Position in world space. Assume that the position is (0,0,0) on model creation.
	glm::vec3 position_;

	// True, if model matrix has changed and has to be updated in shaders
	bool has_changed_;

	// Transformations from outside are controlled by the special functions below to remain consistent with units
	void transform(glm::mat4 transformation_matrix);

	Light::Emitter* emitter_;

	// User-defined ID of the model that this is an instance of
	int id_;

	// Spread of this instance's *bounding-box* in unit space
	float units_x_;
	float units_y_;
	float units_z_;

	std::vector<InstanceAttribute> attribs_;

	ModelInstance(int id, int instance_id, Model* instance_of);

public:
	~ModelInstance();

	void push_attr(InstanceAttribute attrib);
	void attr(int index, InstanceAttribute attrib);
	InstanceAttribute* attr(int index) { return &attribs_[index]; }

	void emit(Light::Emitter l);
	bool is_emitter() { return (emitter_!=0); }

	// GETTER
	int id() { return id_; }
	int instance_id() { return instance_id_; }
	Model* instance_of() { return instance_of_; }
	glm::mat4* model_matrix();
	float unitsX() { return units_x_; }
	float unitsY() { return units_y_; }
	float unitsZ() { return units_z_; }
	bool has_changed() { return has_changed_; }

	// get position in *unit space*
	glm::vec3 position() {
		return glm::vec3(
			position_.x/config::one_unit_x,
			position_.y/config::one_unit_y,
			position_.z/config::one_unit_z);
	}
	float positionX() { return position_.x/config::one_unit_x; }
	float positionY() { return position_.y/config::one_unit_y; }
	float positionZ() { return position_.z/config::one_unit_z; }
	glm::vec3 position_world_space() { return position_; }

	// SETTER
	void was_updated();
	ModelInstance* position(glm::vec3 pos_in_units_from_origin);
	ModelInstance* position(float x, float y, float z);
	ModelInstance* unitsX(float units); // preserves length ratios
	ModelInstance* unitsY(float units); // preserves length ratios
	ModelInstance* unitsZ(float units); // preserves length ratios
	ModelInstance* units(float x_units, float y_units, float z_units); // does not preserve length ratios

	// Transform shorthand functions (return self for chaining)
	ModelInstance* rotate(float angle, glm::vec3 axis);
	ModelInstance* rotateX(float angle);
	ModelInstance* rotateY(float angle);
	ModelInstance* rotateZ(float angle);
	//TODO add rotation with a *transform origin* (carefully update position_ afterwards!)
	ModelInstance* translate(glm::vec3 distances_in_units);
	ModelInstance* translate(float x, float y, float z);
	ModelInstance* translateX(float distance_in_units);
	ModelInstance* translateY(float distance_in_units);
	ModelInstance* translateZ(float distance_in_units);
	ModelInstance* scale(float factor); // preserves length ratios
	ModelInstance* scaleX(float factor); // does not preserve length ratios
	ModelInstance* scaleY(float factor); // does not preserve length ratios
	ModelInstance* scaleZ(float factor); // does not preserve length ratios

	friend class Model; // allows models to see private members
};

#endif