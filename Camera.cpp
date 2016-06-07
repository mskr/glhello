#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up_vector,
  float fov, float screen_ratio, float near, float far) {
	position_ = position;
	target_ = target;
	up_vector_ = up_vector;
	view_projection_matrices_[0] = glm::lookAt(position_, target_, up_vector_);
	view_projection_matrices_[1] = glm::perspective(glm::radians(fov), screen_ratio, near, far);
}

Camera::~Camera() {

}

void Camera::shoot(World* world) {
	//TODO view matrix reset only on changes
	view_projection_matrices_[0] = glm::lookAt(position_, target_, up_vector_);
	post_processor_.pass_1();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->draw();
	post_processor_.pass_2();
}

void Camera::use(PostProcessor p) {
	post_processor_ = p;
}

std::vector<Uniform> Camera::uniforms() {
	return std::vector<Uniform> {
		Uniform("view_projection", view_projection_matrices_, sizeof(view_projection_matrices_), [](Uniform* u) {
			u->update();
		})
	};
}

Interaction* Camera::interaction_type() {
	return new CameraInteraction();
}

void Camera::interact(Interaction* i) {
	CameraInteraction* interaction = (CameraInteraction*)i;
	if     (interaction->type == interaction->SIMPLE)       simple(&interaction->simple_interaction);
	else if(interaction->type == interaction->ARCBALL)      arcball(&interaction->arcball_interaction);
	else if(interaction->type == interaction->FIRST_PERSON) first_person(&interaction->first_person_interaction);
}

void Camera::simple(CameraInteraction::Simple* interaction) {
	if     (interaction->move == interaction->MOVE_RIGHT)    { position_.x++; target_.x++; }
	else if(interaction->move == interaction->MOVE_LEFT)     { position_.x--; target_.x--; }
	else if(interaction->move == interaction->MOVE_UP)       { position_.y++; target_.y++; }
	else if(interaction->move == interaction->MOVE_DOWN)     { position_.y--; target_.y--; }
	else if(interaction->move == interaction->MOVE_FORWARD)  { position_.z++; }
	else if(interaction->move == interaction->MOVE_BACKWARD) { position_.z--; }
}

void Camera::arcball(CameraInteraction::Arcball* interaction) {
	//TODO
}

void Camera::first_person(CameraInteraction::FirstPerson* interaction) {
	//TODO
}