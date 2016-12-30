#include "ShadowMapping.h"

//TODO
// 1) Multiple lightsources
// 2) Point lightsources
// 3) Filtering

ShadowMapping::ShadowMapping(glm::vec3 lightsource_worldspace, glm::vec3 lookAtTarget) : 
	depth_framebuffer_(1024, 1024, {
		std::make_tuple(&depth_texture_, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT)
	})
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border[] = {1,1,1,1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

	depth_map_resolution_ = glm::vec2(1024, 1024);

	//TODO The viewport of which camera should be saved?
	//TODO Update: It's probably better if the camera calls glViewport in loop
	GLint viewport[4]; // x,y,width,height
	glGetIntegerv(GL_VIEWPORT, viewport);
	original_resolution_ = glm::vec2(viewport[2], viewport[3]);

	// Configure depth map shader
	Module::shader_.add_uniform("0", "mat4", "lightSpaceMatrix");
	Module::shader_.add_matrix("lightSpaceMatrix");
	Module::shader_.compile_and_link();

	// Create light space matrices
	float near = 1.0f;
	float far = 1000.0f;
	glm::mat4 view = glm::lookAt(lightsource_worldspace, lookAtTarget, glm::vec3(0,1,0)); //TODO position updates?
	// glm::mat4 proj = glm::ortho(/*left*/-100.0f, /*right*/100.0f, /*bot*/-100.0f, /*top*/100.0f, near, far);
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), depth_map_resolution_.x / depth_map_resolution_.y, near, far);
	light_space_matrix_ = proj * view;

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
	// GPUBuffer vbo(0);
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
	return 1;
	// return light_->num_sources();
}

GLuint ShadowMapping::rendertarget(int pass) {
	return depth_framebuffer_.opengl_id(); //TODO One depth map per light source
}

void ShadowMapping::on_pass(int pass) {
	glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(light_space_matrix_));
	glViewport(0, 0, depth_map_resolution_.x, depth_map_resolution_.y);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void ShadowMapping::after_pass(int pass) {
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
			GLint unit = 0;
			glActiveTexture(GL_TEXTURE0 + unit);
			glBindTexture(GL_TEXTURE_2D, depth_texture_); //TODO create special texture uniforms
			u->update(unit);
		})
	};
}