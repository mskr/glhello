#include "VolumetricLightScatteringMitchell.h"

VolumetricLightScatteringMitchell::VolumetricLightScatteringMitchell(Camera::PostProcessor* p, glm::vec3 lightsource_worldspace) :
	occlusion_framebuffer_(p->camera()->viewport().x, p->camera()->viewport().y, {
		std::make_tuple(&occlusion_texture_, GL_COLOR_ATTACHMENT0, GL_RGB, GL_UNSIGNED_BYTE)
	})
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Do this here because occlusion_texture_ is still bound
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //TODO Better use glTextureParameteri(occlusion_texture_,...)

	post_processor_ = p;
	lightsource_worldspace_ = lightsource_worldspace;
	// Post pass uniform values
	NUM_SAMPLES_ = 100;
	EXPOSURE_ = 0.0034f;
	DENSITY_ = 0.84f;
	WEIGHT_ = 5.65f;
	DECAY_ = 1.0f;

	Module::shader_.add_attribute("int", "Emitter");
	Module::shader_.add_varying("flat", "int", "Emitter", "fEmitter");
	Module::shader_.add_uniform_block("Camera", {{"mat4", "view"}, {"mat4", "proj"}});
	Module::shader_.add_matrix("proj * view");
	Module::shader_.add_storage_buffer("light", {{"float", "lights[]"}});
	//TODO Better check for occluders?
	Module::shader_.set_frag_color("(fEmitter < 0) ? vec4(0) : vec4(lights[6*fEmitter+3], lights[6*fEmitter+4], lights[6*fEmitter+5], 1)");
	Module::shader_.compile_and_link();
}

VolumetricLightScatteringMitchell::~VolumetricLightScatteringMitchell() {
	// Destructor
}

std::vector<Uniform> VolumetricLightScatteringMitchell::uniforms() {
	post_processor_->sampler(1, occlusion_texture_);
	post_processor_->uniform("lightsource", [this](GLint location) {
		glm::vec2 l = post_processor_->camera()->transform_screen_to_texturespace(
			post_processor_->camera()->transform_world_to_screenspace(glm::vec4(this->lightsource_worldspace_, 1))
		);
		glUniform2f(location, l.x, l.y);
	});
	post_processor_->uniform("NUM_SAMPLES", [this](GLint location) {
		glUniform1i(location, this->NUM_SAMPLES_);
	});
	post_processor_->uniform("EXPOSURE", [this](GLint location) {
		glUniform1f(location, this->EXPOSURE_);
	});
	post_processor_->uniform("DENSITY", [this](GLint location) {
		glUniform1f(location, this->DENSITY_);
	});
	post_processor_->uniform("WEIGHT", [this](GLint location) {
		glUniform1f(location, this->WEIGHT_);
	});
	post_processor_->uniform("DECAY", [this](GLint location) {
		glUniform1f(location, this->DECAY_);
	});
	return {};
}

int VolumetricLightScatteringMitchell::num_passes() {
	return 1;
}

GLuint VolumetricLightScatteringMitchell::rendertarget(int pass) {
	return occlusion_framebuffer_.opengl_id();
}

void VolumetricLightScatteringMitchell::on_pass(int pass) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void VolumetricLightScatteringMitchell::debug_pass(int pass) {
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