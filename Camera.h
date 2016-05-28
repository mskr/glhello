#ifndef CAMERA_H_
#define CAMERA_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtc/matrix_access.hpp>
#include <gtc/quaternion.hpp>

#include "World.h"
#include "CameraInteraction.h"
#include "Module.h"

/*
* This is a camera.
* You can place a camera in a world.
* The camera can shoot the world.
* Then the world gets rendered to your screen.
* The image of the world depends on
* a) viewpoint
* b) projection
* which is defined by the camera parameters.
* Camera construction makes "view_projection" available to shaders.
* A camera can be controlled by a user.
*/
class Camera : public Module {

	glm::vec3 position_;
	glm::vec3 target_;
	glm::vec3 up_vector_;

	glm::mat4 view_projection_matrices_[2];

	// These methods receive events of respective interaction type
	void simple(CameraInteraction::Simple* interaction);
	void arcball(CameraInteraction::Arcball* interaction);
	void first_person(CameraInteraction::FirstPerson* interaction);

public:
	Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up_vector,
		float field_of_view, float screen_ratio, float near_clipping_pane, float far_clipping_pane);
	~Camera();

	void shoot(World* world);

	// Overrides
	void interact(Interaction* interaction);
	Interaction* interaction_type();
	std::vector<Uniform> uniforms();

	//TODO Bind a model to the camera, that is always at the same position (e.g. a first-person player)
	//void model(Model m);

};


#endif