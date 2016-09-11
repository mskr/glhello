#include "VolumetricLightScatteringMitchell.h"

VolumetricLightScatteringMitchell::VolumetricLightScatteringMitchell(Camera::PostProcessor* p, glm::vec3 lightsource_worldspace) :
	occlusion_framebuffer_(p->camera()->viewport().x, p->camera()->viewport().y, {
		std::make_tuple(&occlusion_texture_, GL_COLOR_ATTACHMENT0, GL_RGB, GL_UNSIGNED_BYTE)
	})
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Do this here because world_image is still bound
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //TODO Better use glTextureParameteri(world_image_,...)

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
	Module::shader_.set_frag_color("(fEmitter < 0) ? vec4(0) : vec4(lights[6*fEmitter+3], lights[6*fEmitter+4], lights[6*fEmitter+5], 1)");
	Module::shader_.compile_and_link();


	// Debug: 
	// 	GLfloat quad[] {
	// 		// (x, y)      // (u, v)
	// 		-1.0f,  1.0f,  0.0f, 1.0f, // top left
	// 		1.0f, -1.0f,  1.0f, 0.0f, // bottom right
	// 		-1.0f, -1.0f,  0.0f, 0.0f, // bottom left
	// 		-1.0f,  1.0f,  0.0f, 1.0f, // top left
	// 		1.0f,  1.0f,  1.0f, 1.0f, // top right
	// 		1.0f, -1.0f,  1.0f, 0.0f // bottom right
	// 	};
	// 	glGenVertexArrays(1, &debug_vao_);
	// 	glBindVertexArray(debug_vao_);
	// 	GPUBuffer vbo(sizeof(quad), quad);
	// 	debug_shader_ = Shader::link({VertexShader("DepthMapRender.vert"), FragmentShader("DepthMapRender.frag")});
	// 	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	// 	glEnableVertexAttribArray(0);
	// glEnableVertexAttribArray(1);
}

VolumetricLightScatteringMitchell::~VolumetricLightScatteringMitchell() {
	// Destructor
}

std::vector<Uniform> VolumetricLightScatteringMitchell::uniforms() {
	post_processor_->sampler(occlusion_texture_);
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
	// glBindVertexArray(debug_vao_);
	// 	glUseProgram(debug_shader_);
	// 	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// 	glClear(GL_COLOR_BUFFER_BIT); // no need for depth clearing as quad has no depth anyway
	// 	glDisable(GL_DEPTH_TEST); // no need for depth testing
	// 	glActiveTexture(GL_TEXTURE0);
	// 	glBindTexture(GL_TEXTURE_2D, occlusion_texture_);
	// 	glDrawArrays(GL_TRIANGLES, 0, 6);
	// glBindVertexArray(0);
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