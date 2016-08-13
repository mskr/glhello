#ifndef VOLUMETRIC_LIGHT_SCATTERING_MITCHELL_H_
#define VOLUMETRIC_LIGHT_SCATTERING_MITCHELL_H_

#include "../Module.h"
#include "../Interaction.h"
#include "../Shader.h"
#include "../VertexShader.h"
#include "../FragmentShader.h"
#include "Camera.h"

class VolumetricLightScatteringMitchell : public Module {

	unsigned int pre_pass_on_;
	GLuint occlusion_framebuffer_;
	GLuint occlusion_texture_;
	Camera* camera_;
	glm::vec3 lightsource_worldspace_;

	int NUM_SAMPLES_;
	float EXPOSURE_;
	float DENSITY_;
	float WEIGHT_;
	float DECAY_;

	glm::vec2 transform_screen_to_texturespace(glm::vec2 screen_coordinates);

public:
	VolumetricLightScatteringMitchell(Camera* camera, glm::vec3 lightsource_worldspace);
	~VolumetricLightScatteringMitchell();

	std::vector<Uniform> uniforms() override;
	void interact(Interaction* interaction) override;
	Interaction* interaction_type() override;
};

struct VolumetricLightScatteringMitchellInteraction : public Interaction {
	int change_uniform = 0;
	bool increase = true;
	const int change_NUM_SAMPLES = 1;
	const int change_EXPOSURE = 2;
	const int change_DENSITY = 3;
	const int change_WEIGHT = 4;
	const int change_DECAY = 5;
	bool keypress(char key) {
		switch(key) {
			case '1': change_uniform = change_NUM_SAMPLES; increase = false; return true;
			case '2': change_uniform = change_NUM_SAMPLES; increase = true; return true;
			case '3': change_uniform = change_EXPOSURE; increase = false; return true;
			case '4': change_uniform = change_EXPOSURE; increase = true; return true;
			case '5': change_uniform = change_DENSITY; increase = false; return true;
			case '6': change_uniform = change_DENSITY; increase = true; return true;
			case '7': change_uniform = change_WEIGHT; increase = false; return true;
			case '8': change_uniform = change_WEIGHT; increase = true; return true;
			case '9': change_uniform = change_DECAY; increase = false; return true;
			case '0': change_uniform = change_DECAY; increase = true; return true;
			default: return false;
		}
	}
	bool keyrepeat(char key) {
		return keypress(key);
	}
};

#endif