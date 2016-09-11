#ifndef CAMERAINTERACTION_H_
#define CAMERAINTERACTION_H_

#include "../Interaction.h"

/*
* This is the interaction type for the camera module.
*/
struct CameraInteraction : public Interaction {
	bool close_window = false;
	bool escape_key() override { close_window = true; return true; }
	int type = 0;
	const int SIMPLE = 1;
	const int ARCBALL = 2;
	const int FIRST_PERSON = 3;
	void simple() { type = SIMPLE; }
	void arcball() { type = ARCBALL; }
	void first_person() { type = FIRST_PERSON; }
	// parameters for simple interaction type
	struct Simple {
		int move = 0;
		const int MOVE_RIGHT = 1;
		const int MOVE_LEFT = 2;
		const int MOVE_UP = 3;
		const int MOVE_DOWN = 4;
		const int MOVE_FORWARD = 5;
		const int MOVE_BACKWARD = 6;
		void move_right() { move = MOVE_RIGHT; }
		void move_left() { move = MOVE_LEFT; }
		void move_up() { move = MOVE_UP; }
		void move_down() { move = MOVE_DOWN; }
		void move_forward() { move = MOVE_FORWARD; }
		void move_backward() { move = MOVE_BACKWARD; }
		bool keypress(char key) {
			switch(key) {
				case 'q': move_up(); return true;
				case 'a': move_left(); return true;
				case 'e': move_down(); return true;
				case 'd': move_right(); return true;
				case 'w': move_forward(); return true;
				case 's': move_backward(); return true;
				default: return false;
			}
		}
	} simple_interaction;
	//TODO arcball parameters
	struct Arcball {
		
	} arcball_interaction;
	//TODO first_person parameters
	struct FirstPerson {

	} first_person_interaction;


	// === OVERRIDES === //

	bool keypress(char key) {
		if(type == SIMPLE) return simple_interaction.keypress(key);
		else return false;
	}

	bool keyrepeat(char key) {
		if(type == SIMPLE) return simple_interaction.keypress(key);
		else return false;
	}
};

#endif