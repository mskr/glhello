#include "User.h"

std::vector<User*> User::instances = {};

User::User(World* w) {
	world_ = w;
	modules_ = {};
	interactions_ = {};
	User::instances.push_back(this);
}

User::~User() {
	for(Interaction* i : interactions_) delete i;
}

void User::listen_to(GLFWwindow* window) {
	glfwSetKeyCallback(window, User::InputEntryPoints::key);
	glfwSetScrollCallback(window, User::InputEntryPoints::scroll);
	glfwSetCursorPosCallback(window, User::InputEntryPoints::mousemove);
	glfwSetMouseButtonCallback(window, User::InputEntryPoints::click);
}

Interaction* User::use(Module* m) {
	modules_.push_back(m);
	interactions_.push_back(m->interaction_type());
	return interactions_.back();
}

void User::key(int key, int scancode, int action, int mods) {
	switch(key) {
		case GLFW_KEY_SPACE: keyaction(action, ' ', mods); break;
		case GLFW_KEY_0: keyaction(action, '0', mods); break;
		case GLFW_KEY_1: keyaction(action, '1', mods); break;
		case GLFW_KEY_2: keyaction(action, '2', mods); break;
		case GLFW_KEY_3: keyaction(action, '3', mods); break;
		case GLFW_KEY_4: keyaction(action, '4', mods); break;
		case GLFW_KEY_5: keyaction(action, '5', mods); break;
		case GLFW_KEY_6: keyaction(action, '6', mods); break;
		case GLFW_KEY_7: keyaction(action, '7', mods); break;
		case GLFW_KEY_8: keyaction(action, '8', mods); break;
		case GLFW_KEY_9: keyaction(action, '9', mods); break;
		case GLFW_KEY_A: keyaction(action, 'a', mods); break;
		case GLFW_KEY_B: keyaction(action, 'b', mods); break;
		case GLFW_KEY_C: keyaction(action, 'c', mods); break;
		case GLFW_KEY_D: keyaction(action, 'd', mods); break;
		case GLFW_KEY_E: keyaction(action, 'e', mods); break;
		case GLFW_KEY_F: keyaction(action, 'f', mods); break;
		case GLFW_KEY_G: keyaction(action, 'g', mods); break;
		case GLFW_KEY_H: keyaction(action, 'h', mods); break;
		case GLFW_KEY_I: keyaction(action, 'i', mods); break;
		case GLFW_KEY_J: keyaction(action, 'j', mods); break;
		case GLFW_KEY_K: keyaction(action, 'k', mods); break;
		case GLFW_KEY_L: keyaction(action, 'l', mods); break;
		case GLFW_KEY_M: keyaction(action, 'm', mods); break;
		case GLFW_KEY_N: keyaction(action, 'n', mods); break;
		case GLFW_KEY_O: keyaction(action, 'o', mods); break;
		case GLFW_KEY_P: keyaction(action, 'p', mods); break;
		case GLFW_KEY_Q: keyaction(action, 'q', mods); break;
		case GLFW_KEY_R: keyaction(action, 'r', mods); break;
		case GLFW_KEY_S: keyaction(action, 's', mods); break;
		case GLFW_KEY_T: keyaction(action, 't', mods); break;
		case GLFW_KEY_U: keyaction(action, 'u', mods); break;
		case GLFW_KEY_V: keyaction(action, 'v', mods); break;
		case GLFW_KEY_W: keyaction(action, 'w', mods); break;
		case GLFW_KEY_X: keyaction(action, 'x', mods); break;
		case GLFW_KEY_Y: keyaction(action, 'y', mods); break;
		case GLFW_KEY_Z: keyaction(action, 'z', mods); break;
		default: keyaction(action, key, mods);
		//TODO extend list (user should be able to distribute all key inputs)
	}
}

void User::click(int button, int action, int mods) {
	//TODO use the modifier keys too
	switch(action) {
		case GLFW_PRESS: mousepress( button ); break;
		case GLFW_RELEASE: mouserelease( button );
	}
}

void User::mousemove(double xpos, double ypos) {
	for(unsigned int i = 0; i < interactions_.size(); i++)
		if(interactions_[i]->mousemove(xpos, ypos)) modules_[i]->interact(interactions_[i]);
}

void User::scroll(double xoffset, double yoffset) {
	for(unsigned int i = 0; i < interactions_.size(); i++)
		if(interactions_[i]->scroll(xoffset, yoffset)) modules_[i]->interact(interactions_[i]);
}




// Private helper functions

void User::keyaction(int action, char key, int mods) {
	//TODO use the modifier keys too
	switch(action) {
		case GLFW_PRESS: keypress( key ); break;
		case GLFW_REPEAT: keyrepeat( key ); break;
		case GLFW_RELEASE: keyrelease( key );
	}
}

void User::keyaction(int action, int key, int mods) {
	if(key == GLFW_KEY_ESCAPE) {
		for(unsigned int i = 0; i < interactions_.size(); i++)
			if(interactions_[i]->escape_key()) modules_[i]->interact(interactions_[i]);
	}
	//TODO more keys...
}

void User::keypress(char key) {
	for(unsigned int i = 0; i < interactions_.size(); i++)
		if(interactions_[i]->keypress(key)) modules_[i]->interact(interactions_[i]);
}

void User::keyrelease(char key) {
	for(unsigned int i = 0; i < interactions_.size(); i++)
		if(interactions_[i]->keyrelease(key)) modules_[i]->interact(interactions_[i]);
}

void User::keyrepeat(char key) {
	for(unsigned int i = 0; i < interactions_.size(); i++)
		if(interactions_[i]->keyrepeat(key)) modules_[i]->interact(interactions_[i]);
}

void User::mousepress(int button) {
	for(unsigned int i = 0; i < interactions_.size(); i++)
		if(interactions_[i]->click(button)) modules_[i]->interact(interactions_[i]);
}

void User::mouserelease(int button) {
	for(unsigned int i = 0; i < interactions_.size(); i++)
		if(interactions_[i]->cluck(button)) modules_[i]->interact(interactions_[i]);
}