#ifndef MODEL_INSTANCE_H_
#define MODEL_INSTANCE_H_

#include <glm.hpp>
#include <gtx/transform.hpp>

#include "config.h"

//TODO Remove dependence by letting light define a model superclass overriding use() to produce instances with emit() feature
#include "Modules/Light.h"

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

	// User-defined ID of the model that this is an instance of
	int id_;

	// Spread of this instance's *bounding-box* in unit space
	float units_x_;
	float units_y_;
	float units_z_;

	// Hold attributes, including their values
	std::vector<InstanceAttribute> attribs_;

	// Special attribute indicating if this instance emits light (dependence on module should be an exception)
	Light::Emitter* emitter_;

	// Holding pointers to child model instances for hierachic modelling
	std::vector<ModelInstance*> sub_;

	// Construction of instances exclusively in Model::use()
	ModelInstance(int id, int instance_id, Model* instance_of);

public:
	~ModelInstance();

	ModelInstance* attr(unsigned int index, InstanceAttribute attrib);
	void emit(unsigned int index, Light::Emitter emitter);

	// GETTER
	int id() { return id_; }
	int instance_id() { return instance_id_; }
	Model* instance_of() { return instance_of_; }
	glm::mat4* model_matrix();
	InstanceAttribute* attr(unsigned int index) { return &(attribs_[index]); };
	glm::vec3 units() { return glm::vec3(units_x_, units_y_, units_z_); }
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
	ModelInstance* units(glm::vec3 units); // does not preserve length ratios

	// Transform shorthand functions (return self for chaining)
	ModelInstance* rotate(float degrees, glm::vec3 axis);
	ModelInstance* rotateX(float degrees);
	ModelInstance* rotateY(float degrees);
	ModelInstance* rotateZ(float degrees);
	ModelInstance* rotate(float degrees, glm::vec3 axis, glm::vec3 transform_origin);
	ModelInstance* translate(glm::vec3 distances_in_units);
	ModelInstance* translate(float x, float y, float z);
	ModelInstance* translateX(float distance_in_units);
	ModelInstance* translateY(float distance_in_units);
	ModelInstance* translateZ(float distance_in_units);
	ModelInstance* scale(float factor); // preserves length ratios
	ModelInstance* scaleX(float factor); // does not preserve length ratios
	ModelInstance* scaleY(float factor); // does not preserve length ratios
	ModelInstance* scaleZ(float factor); // does not preserve length ratios

	// Hierarchic modelling (return self)
	ModelInstance* sub(std::initializer_list<ModelInstance*> children);

	friend class Model; // allows models to see private members
};

#endif