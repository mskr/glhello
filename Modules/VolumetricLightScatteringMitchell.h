#ifndef VOLUMETRIC_LIGHT_SCATTERING_MITCHELL_H_
#define VOLUMETRIC_LIGHT_SCATTERING_MITCHELL_H_

#include "../Module.h"
#include "../Shader.h"
#include "../VertexShader.h"
#include "../FragmentShader.h"
#include "Camera.h"

class VolumetricLightScatteringMitchell : public Module {

	unsigned int pre_pass_on_;

	GLuint occlusion_framebuffer_;
	GLuint occlusion_texture_;

	glm::vec2 transform_screen_to_texturespace(glm::vec2 screen_coordinates);

public:
	VolumetricLightScatteringMitchell(Camera* camera, glm::vec3 lightsource_worldspace);
	~VolumetricLightScatteringMitchell();

	std::vector<Uniform> uniforms() override;
};

#endif