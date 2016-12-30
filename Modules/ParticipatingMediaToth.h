#ifndef PARTICIPATING_MEDIA_TOTH_H_
#define PARTICIPATING_MEDIA_TOTH_H_

#include "../Module.h"

class ParticipatingMediaToth : public Module {

	int n_;
	float P_;
	float albedo_;
	float density_;

	float g_;

	float scaling_;

public:
	ParticipatingMediaToth();
	~ParticipatingMediaToth();

	std::vector<Uniform> uniforms() override;
	Interaction* interaction_type() override;
	void interact(Interaction* i) override;

};

struct ParticipatingMediaTothInteraction : public Interaction {
	int change_uniform = 0;
	bool increase = true;
	const int change_n = 1;
	const int change_P = 2;
	const int change_albedo = 3;
	const int change_density = 4;
	const int change_g = 5;
	const int change_scale = 6;
	bool keypress(char key) {
		switch(key) {
			case '1': change_uniform = change_n; increase = false; return true;
			case '2': change_uniform = change_n; increase = true; return true;
			case '3': change_uniform = change_P; increase = false; return true;
			case '4': change_uniform = change_P; increase = true; return true;
			case '5': change_uniform = change_albedo; increase = false; return true;
			case '6': change_uniform = change_albedo; increase = true; return true;
			case '7': change_uniform = change_density; increase = false; return true;
			case '8': change_uniform = change_density; increase = true; return true;
			case '9': change_uniform = change_g; increase = false; return true;
			case '0': change_uniform = change_g; increase = true; return true;
			case 'o': change_uniform = change_scale; increase = false; return true;
			case 'p': change_uniform = change_scale; increase = true; return true;
			default: return false;
		}
	}
	bool keyrepeat(char key) {
		return keypress(key);
	}
};

#endif