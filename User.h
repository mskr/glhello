#ifndef USER_H_
#define USER_H_

#include "World.h"
#include "Camera.h"

/*
* This is a user representaion.
* It encapsulates input event processing.
* Input is routed to Interaction objects.
* Every Interaction object interprets input for its needs.
* Then the interaction is sent to a receiver.
* Modules can be receivers by implementing an interact() method.
* Modules have corresponding Interaction objects.
* (Currently using GLFW)
*/
class User {

	// Store instances so that input events can be routed to each of them
	static std::vector<User*> instances;

	// GLFW callbacks have to be either global or static
	static const struct InputEntryPoints {
		static void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
			for(User* user : instances) user->key(key, scancode, action, mods);
		}
		static void scroll(GLFWwindow* window, double xoffset, double yoffset) {
			for(User* user : instances) user->scroll(xoffset, yoffset);
		}
		static void mousemove(GLFWwindow* window, double xpos, double ypos) {
			for(User* user : instances) user->mousemove(xpos, ypos);
		}
		static void click(GLFWwindow* window, int button, int action, int mods) {
			for(User* user : instances) user->click(button, action, mods);
		}
	} input_entry_points;

	World* world_;

	GLFWwindow* window_;

	void keyaction(int action, char key, int mods);
	void keypress(char key);
	void keyrelease(char key);
	void keyrepeat(char key);

	void mousepress(int button);
	void mouserelease(int button);

	std::vector<Module*> modules_;
	std::vector<Interaction*> interactions_;

public:
	User(World* w);
	~User();

	void key(int key, int scancode, int action, int mods);
	void scroll(double xoffset, double yoffset);
	void mousemove(double xpos, double ypos);
	void click(int button, int action, int mods);

	void use(GLFWwindow* window);

	Interaction* use(Module* m);

	//TODO add fast callbacks
	// void on(char key, std::functions<void(void)> action);
	// void on(std::string keycombo, std::functions<void(void)> action);
	
};

#endif