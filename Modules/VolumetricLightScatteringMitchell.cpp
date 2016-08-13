#include "VolumetricLightScatteringMitchell.h"

VolumetricLightScatteringMitchell::VolumetricLightScatteringMitchell(Camera* camera, glm::vec3 lightsource_worldspace) {
	pre_pass_on_ = 0;
	occlusion_framebuffer_ = 0;
	occlusion_texture_ = 0;
	camera_ = camera;
	lightsource_worldspace_ = lightsource_worldspace;
	NUM_SAMPLES_ = 100;
	EXPOSURE_ = 0.0034f;
	DENSITY_ = 0.84f;
	WEIGHT_ = 5.65f;
	DECAY_ = 1.0f;
}

VolumetricLightScatteringMitchell::~VolumetricLightScatteringMitchell() {
}

glm::vec2 VolumetricLightScatteringMitchell::transform_screen_to_texturespace(glm::vec2 screen_coordinates) {
	// texture space is [0,1]^2
	return glm::vec2(screen_coordinates.x/config::viewport_width, screen_coordinates.y/config::viewport_height);
}

std::vector<Uniform> VolumetricLightScatteringMitchell::uniforms() {

	Camera::PostProcessor* p = camera_->post_process(
		Shader::link({VertexShader("PostProcess.vert"), FragmentShader("PostProcess.frag")}),
		{"world_image", "occlusion_pre_pass_image", "lightsource", "NUM_SAMPLES", "EXPOSURE", "DENSITY", "WEIGHT", "DECAY"},
		[this](std::vector<GLint>* uniforms) {
			glUniform1i(uniforms->at(0), 0);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, occlusion_texture_);
			glUniform1i(uniforms->at(1), 1);
			glm::vec2 lightsource = transform_screen_to_texturespace(
				this->camera_->transform_world_to_screenspace(glm::vec4(this->lightsource_worldspace_, 1))
			);
			glUniform2f(uniforms->at(2), lightsource.x, lightsource.y);
			glUniform1i(uniforms->at(3), this->NUM_SAMPLES_);
			glUniform1f(uniforms->at(4), this->EXPOSURE_);
			glUniform1f(uniforms->at(5), this->DENSITY_);
			glUniform1f(uniforms->at(6), this->WEIGHT_);
			glUniform1f(uniforms->at(7), this->DECAY_);
	});

	p->add_pre_pass([this]() {
		pre_pass_on_ = 1; // Switch ON OcclusionPrePass
		glBindFramebuffer(GL_FRAMEBUFFER, occlusion_framebuffer_); // Render to occlusion_texture_
		glEnable(GL_DEPTH_TEST);
	}, [this]() {
		pre_pass_on_ = 0; // Switch OFF OcclusionPrePass
	});

	p->allocate_offscreen_rendertarget(&occlusion_framebuffer_, &occlusion_texture_);

	return {
		Uniform("OcclusionPrePass", [this](Uniform* u, Model* m) { u->update(this->pre_pass_on_); })
	};
}

void VolumetricLightScatteringMitchell::interact(Interaction* i) {
	VolumetricLightScatteringMitchellInteraction* interaction = (VolumetricLightScatteringMitchellInteraction*)i;
	if(interaction->change_uniform == interaction->change_NUM_SAMPLES) {
		interaction->increase ? NUM_SAMPLES_ += 10 : NUM_SAMPLES_ -= 10;
	} else if(interaction->change_uniform == interaction->change_EXPOSURE) {
		interaction->increase ? EXPOSURE_ += 0.001f : EXPOSURE_ -= 0.001f;
	} else if(interaction->change_uniform == interaction->change_DENSITY) {
		interaction->increase ? DENSITY_ += 0.01f : DENSITY_ -= 0.01f;
	} else if(interaction->change_uniform == interaction->change_WEIGHT) {
		interaction->increase ? WEIGHT_ += 0.1f : WEIGHT_ -= 0.1f;
	} else if(interaction->change_uniform == interaction->change_DECAY) {
		interaction->increase ? DECAY_ += 0.1f : DECAY_ -= 0.1f;
	}
}

Interaction* VolumetricLightScatteringMitchell::interaction_type() {
	return new VolumetricLightScatteringMitchellInteraction();
}