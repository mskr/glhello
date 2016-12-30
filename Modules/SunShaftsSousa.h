#ifndef SUN_SHAFTS_SOUSA_H_
#define SUN_SHAFTS_SOUSA_H_

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../Module.h"
#include "../Shader.h"
#include "../VertexShader.h"
#include "../FragmentShader.h"
#include "Light.h"
#include "Camera.h"

class SunShaftsSousa : public Module {

	GPUBuffer framebuffer1_;
	GLuint depth_map1_;

	Camera::PostProcessor* post_processor_1_;

	Camera::PostProcessor* post_processor_2_;
	
	GPUBuffer framebuffer2_;
	GLuint depth_map2_;

	Camera::PostProcessor* composition_post_processor_;

	glm::vec3 lightsource_worldspace_;

	float STEPSIZE_1_;
	float STEPSIZE_2_;
	float STEPSIZE_3_;
	float stepsize_;
	float brightness_;
	int TAPS_;

public:
	SunShaftsSousa(Camera::PostProcessor* p, glm::vec3 lightsource_worldspace);
	~SunShaftsSousa();

	std::vector<Uniform> uniforms() override;
	int num_passes() override;
	void on_pass(int pass) override;
	GLuint rendertarget(int pass) override;
	void after_pass(int pass) override;

	void interact(Interaction* interaction) override;
	Interaction* interaction_type() override;

};

struct SunShaftsSousaInteraction : public Interaction {
	int change_uniform = 0;
	bool increase = true;
	const int change_STEPSIZE_1 = 1;
	const int change_STEPSIZE_2 = 2;
	const int change_STEPSIZE_3 = 3;
	const int change_BRIGHTNESS = 4;
	const int change_TAPS = 5;
	bool keypress(char key) {
		switch(key) {
			case '1': change_uniform = change_STEPSIZE_1; increase = false; return true;
			case '2': change_uniform = change_STEPSIZE_1; increase = true; return true;
			case '3': change_uniform = change_STEPSIZE_2; increase = false; return true;
			case '4': change_uniform = change_STEPSIZE_2; increase = true; return true;
			case '5': change_uniform = change_STEPSIZE_3; increase = false; return true;
			case '6': change_uniform = change_STEPSIZE_3; increase = true; return true;
			case '7': change_uniform = change_BRIGHTNESS; increase = false; return true;
			case '8': change_uniform = change_BRIGHTNESS; increase = true; return true;
			case '9': change_uniform = change_TAPS; increase = false; return true;
			case '0': change_uniform = change_TAPS; increase = true; return true;
			default: return false;
		}
	}
	bool keyrepeat(char key) {
		return keypress(key);
	}
};

#endif