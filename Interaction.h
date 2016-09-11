#ifndef INTERACTION_H_
#define INTERACTION_H_

/*
* This is the base class for interaction types.
* It is used as input event.
* It is created by User objects and sent to Modules.
* Important methods are
* a) Interaction* Module::interaction_type()
*    - Must return the interaction type corresponding to this module.
* b) Interaction* User::use(Module*)
*    - Let the user deliver input events to the given module.
* c) void Module::interact(Interaction*)
*    - Handles the event.
* Methods in this class are meant to be overridden.
* Overrides should return true to signal that they interpret the input.
*/
struct Interaction {
	Interaction() {}
	virtual ~Interaction() {}

	virtual bool escape_key() { return false; }

	virtual bool keypress(char key) { return false; } //GLFW_PRESS
	virtual bool keyrepeat(char key) { return false; } //GLFW_REPEAT
	virtual bool keyrelease(char key) { return false; } //GLFW_RELEASE
	virtual bool keypress(char key, char mod) { return false; }
	virtual bool keyrepeat(char key, char mod) { return false; }
	virtual bool keyrelease(char key, char mod) { return false; }

	virtual bool scroll(double xoffset, double yoffset) { return false; }

	virtual bool mousemove(double xpos, double ypos) { return false; }

	virtual bool click(int button) { return false; } //GLFW_PRESS
	virtual bool cluck(int button) { return false; } //GLFW_ERLEASE
	virtual bool click(int button, char mod) { return false; }
	virtual bool cluck(int button, char mod) { return false; }
};

#endif