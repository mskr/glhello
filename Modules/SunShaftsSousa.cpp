#include "SunShaftsSousa.h"

SunShaftsSousa::SunShaftsSousa(Camera::PostProcessor* p, glm::vec3 lightsource_worldspace) :
	framebuffer1_(p->camera()->viewport().x, p->camera()->viewport().y, {
		std::make_tuple(&depth_map1_, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE)
	}),
	framebuffer2_(p->camera()->viewport().x, p->camera()->viewport().y, {
		std::make_tuple(&depth_map2_, GL_COLOR_ATTACHMENT0, GL_RED, GL_UNSIGNED_BYTE)
	})
{
	glBindTexture(GL_TEXTURE_2D, depth_map1_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// float black[] = {0,0,0,1};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
	glBindTexture(GL_TEXTURE_2D, depth_map2_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);

	lightsource_worldspace_ = lightsource_worldspace;

	// Depth map generation shader
	Module::shader_.add_uniform_block("Camera", {{"mat4", "view"}, {"mat4", "proj"}});
	Module::shader_.add_matrix("proj * view");
	Module::shader_.add_uniform("0", "float", "near");
	Module::shader_.add_uniform("1", "float", "far");
	// Linearize depth
	Module::shader_.set_frag_depth("(2.0 * near * far) / (far + near - (gl_FragCoord.z * 2.0 - 1.0) * (far - near)) / far");
	Module::shader_.compile_and_link();

	GLuint blurShader = Shader::link({VertexShader("Sousa.vert"), FragmentShader("Sousa.frag")});
	composition_post_processor_ = p;
	// Post processor 2 uses rendertarget with GL_RED GL_UNSIGNED_BYTE texture as input and outputs in framebuffer2_
	post_processor_2_ = p->new_instance(GL_RED, GL_UNSIGNED_BYTE, framebuffer2_.opengl_id(), blurShader);
	// Post processor 1 uses depth_map_1 as input and outputs to the rendertarget of post processor 2
	post_processor_1_ = p->new_instance(depth_map1_, post_processor_2_->rendertarget(), blurShader);

	stepsize_ = 1.0f;
	TAPS_ = 20.0f;
	STEPSIZE_1_ = pow(TAPS_, -1);
	STEPSIZE_2_ = pow(TAPS_, -2);
	STEPSIZE_3_ = pow(TAPS_, -3);
	brightness_ = 0.19f;
}

SunShaftsSousa::~SunShaftsSousa() {
	//Destructor
	delete post_processor_2_;
	delete post_processor_1_;
}

std::vector<Uniform> SunShaftsSousa::uniforms() {
	// Uniforms for post processor 1
	post_processor_2_->uniform("stepsize", [this](GLint loc) { glUniform1f(loc, stepsize_); });
	post_processor_2_->uniform("TAPS", [this](GLint loc) { glUniform1i(loc, TAPS_); });
	post_processor_2_->uniform("brightness", [this](GLint loc) { glUniform1f(loc, brightness_); });
	// post_processor_2_->uniform("near", [this](GLint loc) {
	// 	Camera* cam = post_processor_2_->camera();
	// 	glUniform1f(loc, cam->near());
	// });
	// post_processor_2_->uniform("far", [this](GLint loc) {
	// 	Camera* cam = post_processor_2_->camera();
	// 	glUniform1f(loc, cam->far());
	// });
	post_processor_2_->uniform("lightsource", [this](GLint loc) {
		Camera* cam = post_processor_2_->camera();
		glm::vec2 l = cam->transform_screen_to_texturespace(cam->transform_world_to_screenspace(
			glm::vec4(lightsource_worldspace_, 1)
		));
		glUniform2f(loc, l.x, l.y);
	});
	// Uniforms for post processor 2
	post_processor_1_->uniform("stepsize", [this](GLint loc) { glUniform1f(loc, stepsize_); });
	post_processor_1_->uniform("TAPS", [this](GLint loc) { glUniform1i(loc, TAPS_); });
	post_processor_1_->uniform("brightness", [this](GLint loc) { glUniform1f(loc, brightness_); });
	// post_processor_1_->uniform("near", [this](GLint loc) {
	// 	Camera* cam = post_processor_1_->camera();
	// 	glUniform1f(loc, cam->near());
	// });
	// post_processor_1_->uniform("far", [this](GLint loc) {
	// 	Camera* cam = post_processor_1_->camera();
	// 	glUniform1f(loc, cam->far());
	// });
	post_processor_1_->uniform("lightsource", [this](GLint loc) {
		Camera* cam = post_processor_1_->camera();
		glm::vec2 l = cam->transform_screen_to_texturespace(cam->transform_world_to_screenspace(
			glm::vec4(lightsource_worldspace_, 1)
		));
		glUniform2f(loc, l.x, l.y);
	});
	// Uniforms for composition post processor
	composition_post_processor_->sampler(2, depth_map2_);
	composition_post_processor_->uniform("stepsize", [this](GLint loc) { glUniform1f(loc, stepsize_); });
	composition_post_processor_->uniform("TAPS", [this](GLint loc) { glUniform1i(loc, TAPS_); });
	composition_post_processor_->uniform("brightness", [this](GLint loc) { glUniform1f(loc, brightness_); });
	// composition_post_processor_->uniform("near", [this](GLint loc) {
	// 	Camera* cam = composition_post_processor_->camera();
	// 	glUniform1f(loc, cam->near());
	// });
	// composition_post_processor_->uniform("far", [this](GLint loc) {
	// 	Camera* cam = composition_post_processor_->camera();
	// 	glUniform1f(loc, cam->far());
	// });
	composition_post_processor_->uniform("lightsource", [this](GLint loc) {
		Camera* cam = composition_post_processor_->camera();
		glm::vec2 l = cam->transform_screen_to_texturespace(cam->transform_world_to_screenspace(
			glm::vec4(lightsource_worldspace_, 1)
		));
		glUniform2f(loc, l.x, l.y);
	});
	// 1st working method for setting near and far uniforms in pre pass shader
	return {Uniform("near", [this](Uniform* u, Model* m) {
			u->update(composition_post_processor_->camera()->near());
		}), Uniform("far", [this](Uniform* u, Model* m) {
			u->update(composition_post_processor_->camera()->far());
		})
	};
}

int SunShaftsSousa::num_passes() {
	return 1;
}

void SunShaftsSousa::on_pass(int pass) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// 2nd working method for setting near and far uniforms in pre pass shader
	// glUniform1f(0, composition_post_processor_->camera()->near());
	// glUniform1f(1, composition_post_processor_->camera()->far());
}

GLuint SunShaftsSousa::rendertarget(int pass) {
	// Step 1): Render world depth to depth_map1_ texture
	return framebuffer1_.opengl_id();
}

void SunShaftsSousa::after_pass(int pass) {
	// Step 2): Run post processor 1, which works on depth_map1_ and renders to post processor 2's rendertarget
	stepsize_ = STEPSIZE_1_;
	post_processor_1_->post_pass();
	// Step 3): Run post processor 2, which works on the texture of his rendertarget and renders to depth_map2_ texture
	stepsize_ = STEPSIZE_2_;
	post_processor_2_->post_pass();
	// Step 4): World is rendered to composition_post_processor_'s rendertarget
	// Step 5): Camera runs composition_post_processor_, which has the world image AND depth_map2_ image and combines them
	stepsize_ = STEPSIZE_3_;
}




void SunShaftsSousa::interact(Interaction* i) {
	SunShaftsSousaInteraction* interaction = (SunShaftsSousaInteraction*)i;
	if(interaction->change_uniform == interaction->change_STEPSIZE_1) {
		interaction->increase ? STEPSIZE_1_ += 0.001f : STEPSIZE_1_ -= 0.001f;
	} else if(interaction->change_uniform == interaction->change_STEPSIZE_2) {
		interaction->increase ? STEPSIZE_2_ += 0.001f : STEPSIZE_2_ -= 0.001f;
	} else if(interaction->change_uniform == interaction->change_STEPSIZE_3) {
		interaction->increase ? STEPSIZE_3_ += 0.001f : STEPSIZE_3_ -= 0.001f;
	} else if(interaction->change_uniform == interaction->change_BRIGHTNESS) {
		interaction->increase ? brightness_ += 0.01f : brightness_ -= 0.01f;
	} else if(interaction->change_uniform == interaction->change_TAPS) {
		interaction->increase ? TAPS_ += 1 : TAPS_ -= 2;
		STEPSIZE_1_ = pow(TAPS_, -1);
		STEPSIZE_2_ = pow(TAPS_, -2);
		STEPSIZE_3_ = pow(TAPS_, -3);
	}
	printf("STEPSIZE_1_ = %f, STEPSIZE_2_ = %f, STEPSIZE_3_ = %f, brightness_ = %f, TAPS_ = %d\n",
		STEPSIZE_1_, STEPSIZE_2_, STEPSIZE_3_, brightness_, TAPS_);
}

Interaction* SunShaftsSousa::interaction_type() {
	return new SunShaftsSousaInteraction();
}