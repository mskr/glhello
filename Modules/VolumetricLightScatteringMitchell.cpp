#include "VolumetricLightScatteringMitchell.h"

VolumetricLightScatteringMitchell::VolumetricLightScatteringMitchell(Camera* camera, glm::vec3 lightsource_worldspace) {
	pre_pass_on_ = 0;

	occlusion_framebuffer_ = 0;
	occlusion_texture_ = 0;

	Camera::PostProcessor* p = camera->post_process(
		Shader::link({VertexShader("PostProcess.vert"), FragmentShader("PostProcess.frag")}),
		{"world_image", "occlusion_pre_pass_image", "lightsource"}, [this, camera, lightsource_worldspace](std::vector<GLint>* uniforms) {
			// Add godrays_texture_ to texture of ordinary world and render onscreen 
			glUniform1i(uniforms->at(0), 0);
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, occlusion_texture_);
			glUniform1i(uniforms->at(1), 1);
			glm::vec2 lightsource = transform_screen_to_texturespace(
				camera->transform_world_to_screenspace(glm::vec4(lightsource_worldspace, 1))
			);
			glUniform2f(uniforms->at(2), lightsource.x, lightsource.y);
	});

	p->add_pre_pass([this]() {
		pre_pass_on_ = 1; // Switch ON OcclusionPrePass
		glBindFramebuffer(GL_FRAMEBUFFER, occlusion_framebuffer_); // Render to occlusion_texture_
		glEnable(GL_DEPTH_TEST);
	}, [this]() {
		pre_pass_on_ = 0; // Switch OFF OcclusionPrePass
	});

	p->allocate_offscreen_rendertarget(&occlusion_framebuffer_, &occlusion_texture_);
}

VolumetricLightScatteringMitchell::~VolumetricLightScatteringMitchell() {
}

glm::vec2 VolumetricLightScatteringMitchell::transform_screen_to_texturespace(glm::vec2 screen_coordinates) {
	// texture space is [0,1]^2
	return glm::vec2(screen_coordinates.x/config::viewport_width, screen_coordinates.y/config::viewport_height);
}

std::vector<Uniform> VolumetricLightScatteringMitchell::uniforms() {
	return {
		Uniform("OcclusionPrePass", [this](Uniform* u, Model* m) { u->update(this->pre_pass_on_); })
	};
}