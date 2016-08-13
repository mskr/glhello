#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up_vector, float fov) {
	// window_ = glfwCreateWindow(config::viewport_width, config::viewport_height, title, NULL, NULL);
	position_ = position;
	target_ = target;
	up_vector_ = up_vector;
	view_projection_matrices_[0] = glm::lookAt(position_, target_, up_vector_);
	view_projection_matrices_[1] = glm::perspective(
		glm::radians(fov),
		(float)(config::viewport_width/config::viewport_height),
		config::near, config::far
	);
}

Camera::~Camera() {

}

void Camera::shoot(World* world) {
	view_projection_matrices_[0] = glm::lookAt(position_, target_, up_vector_); //TODO view matrix calc only on changes
	if(post_processor.pre_pass()) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		world->draw();
		post_processor.post_pre_pass();
	}
	post_processor.ordinary_pass();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	world->draw();
	post_processor.post_pass();
}

glm::vec2 Camera::transform_world_to_screenspace(glm::vec4 worldspace_coordinates) const {
	// Step 1: Get homogeneous coordinates in frustum.
	glm::vec4 homogeneous = view_projection_matrices_[1] * view_projection_matrices_[0] * worldspace_coordinates;
	// Step 2: Get normalized device coordinates (NDCs) in [-1,1]^3 cube by doing perspective divide.
	glm::vec3 cube = glm::vec3(homogeneous.x, homogeneous.y, homogeneous.z) / homogeneous.w;
	// Step 3: Get screen coordinates by scaling and translating NDCs into viewport (ignoring Z).
	return glm::vec2((1.0f + cube.x) * config::viewport_width / 2.0f, (1.0f + cube.y) * config::viewport_height / 2.0f);
}

std::vector<Uniform> Camera::uniforms() {
	return std::vector<Uniform> {
		Uniform("Camera", view_projection_matrices_, sizeof(view_projection_matrices_), [](Uniform* u) {
			u->update();
		})
	};
}

Interaction* Camera::interaction_type() {
	return new CameraInteraction();
}

void Camera::interact(Interaction* i) {
	CameraInteraction* interaction = (CameraInteraction*)i;
	if (interaction->type == interaction->SIMPLE) {
		simple(&interaction->simple_interaction);
	} else if(interaction->type == interaction->ARCBALL) {
		arcball(&interaction->arcball_interaction);
	} else if(interaction->type == interaction->FIRST_PERSON) {
		first_person(&interaction->first_person_interaction);
	}
}

void Camera::simple(CameraInteraction::Simple* interaction) {
	if (interaction->move == interaction->MOVE_RIGHT) {
		position_.x++;
		target_.x++;
	} else if(interaction->move == interaction->MOVE_LEFT) {
		position_.x--;
		target_.x--;
	} else if(interaction->move == interaction->MOVE_UP) {
		position_.y++;
		target_.y++;
	} else if(interaction->move == interaction->MOVE_DOWN) {
		position_.y--;
		target_.y--;
	} else if(interaction->move == interaction->MOVE_FORWARD) {
		position_.z--;
	} else if(interaction->move == interaction->MOVE_BACKWARD) {
		position_.z++;
	}
}

void Camera::arcball(CameraInteraction::Arcball* interaction) {
	//TODO
}

void Camera::first_person(CameraInteraction::FirstPerson* interaction) {
	//TODO
}



















Camera::PostProcessor* Camera::post_process(
  GLuint post_pass_shader, 
  std::initializer_list<std::string> uniforms, 
  std::function<void(std::vector<GLint>*)> uniform_callback) 
{
	post_processor = PostProcessor(post_pass_shader, uniforms, uniform_callback);
	return &post_processor;
}

GLfloat Camera::PostProcessor::fullscreen_quad_[] = {
	// (x, y)      // (u, v)
	-1.0f,  1.0f,  0.0f, 1.0f, // top left
	1.0f, -1.0f,  1.0f, 0.0f, // bottom right
	-1.0f, -1.0f,  0.0f, 0.0f, // bottom left
	-1.0f,  1.0f,  0.0f, 1.0f, // top left
	1.0f,  1.0f,  1.0f, 1.0f, // top right
	1.0f, -1.0f,  1.0f, 0.0f // bottom right
};

Camera::PostProcessor::PostProcessor() {
	off_ = true;
	framebuffer_ = 0;
	post_pass_shader_ = 0;
	texture_ = 0;
	vao_ = 0;
	pre_pass_ = 0;
}

Camera::PostProcessor::PostProcessor(
  GLuint post_pass_shader, 
  std::initializer_list<std::string> uniforms, 
  std::function<void(std::vector<GLint>*)> uniform_callback)
{
	off_ = false;
	post_pass_shader_ = post_pass_shader;
	for(std::string s : uniforms) locate_post_pass_uniform(s.c_str());
	uniform_callback_ = uniform_callback;
	allocate_offscreen_rendertarget(&framebuffer_, &texture_);
	upload_quad_vertices();
	enable_quad_coords_in_shader(post_pass_shader_);
	pre_pass_ = 0;
}

Camera::PostProcessor::~PostProcessor() {
	// in destructor, delete quad vao?
}

void Camera::PostProcessor::ordinary_pass() {
	if(off_) return;
	// render world to texture
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	glEnable(GL_DEPTH_TEST);
}

void Camera::PostProcessor::post_pass() {
	if(off_) return;
	// render textured quad to default framebuffer (and edit texture in shader)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT); // no need for depth clearing as quad has no depth anyway
	glUseProgram(post_pass_shader_);
	glBindVertexArray(vao_);
	glDisable(GL_DEPTH_TEST); // no need for depth testing
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_);
	uniform_callback_(&uniform_locations_);
	glDrawArrays(GL_TRIANGLES, 0, 6); // invoke post process shader here!
	glBindVertexArray(0);
}

bool Camera::PostProcessor::pre_pass() {
	if(pre_pass_ == 0) return false;
	pre_pass_();
	return true;
}

void Camera::PostProcessor::post_pre_pass() {
	if(post_pre_pass_ == 0) return;
	post_pre_pass_();
}


void Camera::PostProcessor::add_pre_pass(
  std::function<void()> pre_pass,
  std::function<void()> post_pre_pass)
{
	pre_pass_ = pre_pass;
	post_pre_pass_ = post_pre_pass;
}

void Camera::PostProcessor::upload_quad_vertices() {
	// Upload vertices of fullscreen quad
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	GPUBuffer vbo;
	vbo.bind_data(sizeof(fullscreen_quad_), fullscreen_quad_);
}

void Camera::PostProcessor::enable_quad_coords_in_shader(GLuint shader) {
	GLint loc1 = glGetAttribLocation(shader, "position");
	GLint loc2 = glGetAttribLocation(shader, "texCoords");
	glVertexAttribPointer(loc1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(loc1);
	glEnableVertexAttribArray(loc2);
	glBindVertexArray(0);
}

void Camera::PostProcessor::allocate_offscreen_rendertarget(GLuint* framebuffer, GLuint* texture) {
	// Allocate GPU memory for color, depth and stencil values
	glGenFramebuffers(1, framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, *framebuffer);
	// Target for color values (Texture has read-/write-access in shaders)
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		config::viewport_width, config::viewport_height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		(GLvoid*)0 // texture not initialized with data
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texture, 0);
	// Target for depth and stencil values (Renderbuffer is write-only but faster than textures)
	GLuint renderbuffer = 0;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		config::viewport_width, config::viewport_height
	);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
	// Is the framebuffer complete?
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Program exits because PostProcessor failed to complete framebuffer.");
	// Make the default framebuffer active again
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Camera::PostProcessor::locate_post_pass_uniform(const GLchar* uniform_name) {
	GLint loc = glGetUniformLocation(post_pass_shader_, uniform_name);
	if(loc == -1) printf("WARNING: Uniform \"%s\" not found in post process shader.\n", uniform_name);
	uniform_locations_.push_back(loc);
}