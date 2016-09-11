#include "ShadowMapping.h"

ShadowMapping::ShadowMapping(Light* light) : 
	depth_framebuffer_(1024, 1024, {
		std::make_tuple(&depth_texture_, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT)
	})
{
	light_ = light;
	depth_map_resolution_ = glm::vec2(1024, 1024);
	GLint viewport[4]; // x,y,width,height
	glGetIntegerv(GL_VIEWPORT, viewport);
	original_resolution_ = glm::vec2(viewport[2], viewport[3]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Create framebuffer with only a depth attachment
		// glGenFramebuffers(1, &depth_framebuffer_);
		// glGenTextures(1, &depth_texture_);
		// glBindTexture(GL_TEXTURE_2D, depth_texture_);
		// // Allocate texture memory
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depth_map_resolution_.x, depth_map_resolution_.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, (GLvoid*)0);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// // Attach depth texture
		// glBindFramebuffer(GL_FRAMEBUFFER, depth_framebuffer_);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_, 0);
		// // Disable color target
		// glDrawBuffer(GL_NONE); // OpenGL won't try to write to a color buffer
		// glReadBuffer(GL_NONE); // reading from color buffer is also disabled
		// // Check if framebuffer complete
		// if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		// 	throw std::runtime_error("Program exits because ShadowMapping failed to complete framebuffer.");
	// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Configure depth map shader
	Module::shader_.add_uniform("mat4", "lightSpaceMatrix");
	Module::shader_.add_matrix("lightSpaceMatrix");
	Module::shader_.compile_and_link();

	// Debug: render depth map to quad
		// GLfloat quad[] {
		// 	// (x, y)      // (u, v)
		// 	-1.0f,  1.0f,  0.0f, 1.0f, // top left
		// 	1.0f, -1.0f,  1.0f, 0.0f, // bottom right
		// 	-1.0f, -1.0f,  0.0f, 0.0f, // bottom left
		// 	-1.0f,  1.0f,  0.0f, 1.0f, // top left
		// 	1.0f,  1.0f,  1.0f, 1.0f, // top right
		// 	1.0f, -1.0f,  1.0f, 0.0f // bottom right
		// };
		// glGenVertexArrays(1, &debug_vao_);
		// glBindVertexArray(debug_vao_);
		// GPUBuffer vbo;
		// vbo.bind_data(sizeof(quad), quad);
		// debug_shader_ = Shader::link({VertexShader("DepthMapRender.vert"), FragmentShader("DepthMapRender.frag")});
		// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
		// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
		// glEnableVertexAttribArray(0);
	// glEnableVertexAttribArray(1);
}

ShadowMapping::~ShadowMapping() {
	// Destructor
}

int ShadowMapping::num_passes() {
	return light_->num_sources();
}

GLuint ShadowMapping::rendertarget(int pass) {
	return depth_framebuffer_.opengl_id(); //TODO One depth map per light source
}

void ShadowMapping::on_pass(int pass) {
	// Create light space matrices
	float near = 1.0f;
	float far = 20.0f;
	glm::mat4 view = glm::lookAt(light_->position(0), glm::vec3(0), glm::vec3(0,1,0));
	// glm::mat4 proj = glm::ortho(/*left*/-10.0f, /*right*/10.0f, /*bot*/-10.0f, /*top*/10.0f, near, far);
	glm::mat4 proj = glm::perspective(glm::radians(65.0f), depth_map_resolution_.x / depth_map_resolution_.y, near, far);
	light_space_matrix_ = proj * view;
	glUniformMatrix4fv(glGetUniformLocation(Module::shader_.gpu_program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(light_space_matrix_));
	glViewport(0, 0, depth_map_resolution_.x, depth_map_resolution_.y);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void ShadowMapping::debug_pass(int pass) {
	glViewport(0, 0, original_resolution_.x, original_resolution_.y);
	// glBindVertexArray(debug_vao_);
		// glUseProgram(debug_shader_);
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glClear(GL_COLOR_BUFFER_BIT); // no need for depth clearing as quad has no depth anyway
		// glDisable(GL_DEPTH_TEST); // no need for depth testing
		// glActiveTexture(GL_TEXTURE0);
		// glBindTexture(GL_TEXTURE_2D, depth_texture_);
		// glDrawArrays(GL_TRIANGLES, 0, 6);
	// glBindVertexArray(0);
}

std::vector<Uniform> ShadowMapping::uniforms() {
	//TODO Shader Storage Block will be needed to be as dynamic as the light sources
	return std::vector<Uniform> {
		Uniform("ShadowMappingMatrix", [this](Uniform* u, Model* m) {
			u->update(light_space_matrix_);
		}),
		Uniform("ShadowMap", [this](Uniform* u, Model* m) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depth_texture_); //TODO create special texture uniforms
			u->update((int)0);
		})
	};
}