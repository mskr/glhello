#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up_vector, float fov, glm::vec2 viewport, float near, float far) {
	is_on_ = true;
	viewport_ = viewport;
	if(!glfwInit()) throw std::runtime_error("Program exits because GLFW init failed.");
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window_ = glfwCreateWindow(viewport_.x, viewport_.y, config::program_title.c_str(), NULL, NULL);
	glfwMakeContextCurrent(window_);
	User::listen_to(window_);
	position_ = position;
	target_ = target;
	up_vector_ = up_vector;
	near_ = near;
	far_ = far;
	view_projection_matrices_[0] = glm::lookAt(position_, target_, up_vector_);
	view_projection_matrices_[1] = glm::perspective(glm::radians(fov), viewport_.x/viewport_.y, near, far);

	glewExperimental = GL_TRUE;
	glewInit();
}

Camera::~Camera() {
	glfwDestroyWindow(window_);
	glfwTerminate();
}

void Camera::shoot(World* world) {
	view_projection_matrices_[0] = glm::lookAt(position_, target_, up_vector_); //TODO only on changes
	world->draw(post_processor_.rendertarget());
	post_processor_.post_pass();
	glfwSwapBuffers(window_);
}

glm::vec2 Camera::transform_world_to_screenspace(glm::vec4 worldspace_coordinates) const {
	// Step 1: Get homogeneous coordinates in frustum.
	glm::vec4 homogeneous = view_projection_matrices_[1] * view_projection_matrices_[0] * worldspace_coordinates;
	// Step 2: Get normalized device coordinates (NDCs) in [-1,1]^3 cube by doing perspective divide.
	glm::vec3 cube = glm::vec3(homogeneous.x, homogeneous.y, homogeneous.z) / homogeneous.w;
	// Step 3: Get screen coordinates by scaling and translating NDCs into viewport (ignoring Z).
	return glm::vec2((1.0f + cube.x) * viewport_.x / 2.0f, (1.0f + cube.y) * viewport_.y / 2.0f);
}

glm::vec2 Camera::transform_screen_to_texturespace(glm::vec2 screenspace_coordinates) const {
	// Screen-space is the viewport area, texture-space is [0,1]^2
	return glm::vec2(screenspace_coordinates.x/viewport_.x, screenspace_coordinates.y/viewport_.y);
}

std::vector<Uniform> Camera::uniforms() {
	return std::vector<Uniform> {
		Uniform("Camera", view_projection_matrices_, sizeof(view_projection_matrices_), [](Uniform* u) {
			u->update();
		}),
		Uniform("viewport", [this](Uniform* u, Model* m) {
			u->update(viewport_);
		}),
		Uniform("cameraPos", [this](Uniform* u, Model* m) {
			u->update(position_);
		})
	};
}

Interaction* Camera::interaction_type() {
	return new CameraInteraction();
}

void Camera::interact(Interaction* i) {
	CameraInteraction* interaction = (CameraInteraction*)i;
	if(interaction->close_window) {
		is_on_ = false;
	} else if(interaction->type == interaction->SIMPLE) {
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
	//printf("position(%f,%f,%f), target(%f,%f,%f)\n", position_.x,position_.y,position_.z, target_.x,target_.y,target_.z);
}

void Camera::arcball(CameraInteraction::Arcball* interaction) {
	//TODO
}

void Camera::first_person(CameraInteraction::FirstPerson* interaction) {
	//TODO
}

Camera::PostProcessor* Camera::post_processor() {
	if(!post_processor_.is_on_) {
		post_processor_.on(this);
	}
	return &post_processor_;
}

Camera::PostProcessor::PostProcessor() {
	is_on_ = false;
	camera_ = 0;
	framebuffer_ = 0;
	shader_ = 0;
	world_image_ = 0;
	vao_ = 0;
	samplers_ = {};
	uniform_update_functions_ = {};
	output_rendertarget_ = 0;
}

void Camera::PostProcessor::on(Camera* camera) {
	is_on_ = true;
	camera_ = camera;
	framebuffer_ = new GPUBuffer(camera_->viewport_.x, camera_->viewport_.y, {
		std::make_tuple(&world_image_, GL_COLOR_ATTACHMENT0, GL_RGB, GL_UNSIGNED_BYTE)
	});
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Do this here because world_image is still bound
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //TODO Better use glTextureParameteri(world_image_,...)
	shader_ = Shader::link({VertexShader("PostProcess.vert"), FragmentShader("PostProcess.frag")});
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	GLfloat quad[] = {
		// (x, y)      // (u, v)
		-1.0f,  1.0f,  0.0f, 1.0f, // top left
		1.0f, -1.0f,  1.0f, 0.0f, // bottom right
		-1.0f, -1.0f,  0.0f, 0.0f, // bottom left
		-1.0f,  1.0f,  0.0f, 1.0f, // top left
		1.0f,  1.0f,  1.0f, 1.0f, // top right
		1.0f, -1.0f,  1.0f, 0.0f // bottom right
	};
	GPUBuffer vbo(sizeof(quad), quad);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	samplers_[0] = world_image_;
}

Camera::PostProcessor::~PostProcessor() {
	// Destructor
	framebuffer_->null();
	delete framebuffer_;
}

Camera::PostProcessor* Camera::PostProcessor::new_instance(GLint input_format, GLenum input_type, GLuint output_rendertarget, GLuint shader) {
	Camera::PostProcessor* p = new Camera::PostProcessor(); // caller cleans this up
	p->camera_ = this->camera_;
	p->framebuffer_  = new GPUBuffer(camera_->viewport_.x, camera_->viewport_.y, {
		std::make_tuple(&world_image_, GL_COLOR_ATTACHMENT0, input_format, input_type)
	});
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Do this here because world_image is still bound
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //TODO Better use glTextureParameteri(world_image_,...)
	p->samplers_[0] = world_image_;
	p->output_rendertarget_ = output_rendertarget;
	p->shader_ = shader;
	p->vao_ = this->vao_;
	p->is_on_ = true;
	return p;
}

Camera::PostProcessor* Camera::PostProcessor::new_instance(GLuint output_rendertarget, GLuint shader) {
	Camera::PostProcessor* p = new Camera::PostProcessor(); // caller cleans this up
	p->camera_ = this->camera_;
	p->framebuffer_  = new GPUBuffer(camera_->viewport_.x, camera_->viewport_.y, {
		std::make_tuple(&world_image_, GL_COLOR_ATTACHMENT0, GL_RGB, GL_UNSIGNED_BYTE)
	});
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Do this here because world_image is still bound
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //TODO Better use glTextureParameteri(world_image_,...)
	p->samplers_[0] = world_image_;
	p->output_rendertarget_ = output_rendertarget;
	p->shader_ = shader;
	p->vao_ = this->vao_;
	p->is_on_ = true;
	return p;
}

Camera::PostProcessor* Camera::PostProcessor::new_instance(GLuint input_texture, GLuint output_rendertarget, GLuint shader) {
	Camera::PostProcessor* p = new Camera::PostProcessor(); // caller cleans this up
	p->camera_ = this->camera_;
	p->framebuffer_ = 0; //TODO dont need this but nulling is unsafe!
	p->samplers_[0] = input_texture;
	p->output_rendertarget_ = output_rendertarget;
	p->shader_ = shader;
	p->vao_ = this->vao_;
	p->is_on_ = true;
	return p;
}

GLuint Camera::PostProcessor::rendertarget() {
	if(!is_on_) return 0;
	return framebuffer_->opengl_id();
}

void Camera::PostProcessor::post_pass() {
	if(!is_on_) return;
	glBindFramebuffer(GL_FRAMEBUFFER, output_rendertarget_);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shader_);
	glBindVertexArray(vao_);
	for(std::pair<GLint, GLuint> s : samplers_) {
		glActiveTexture(GL_TEXTURE0 + s.first);
		glBindTexture(GL_TEXTURE_2D, s.second);
	}
	for(std::pair<GLint, std::function<void(GLint)>> u : uniform_update_functions_)
		u.second(u.first);
	glDrawArrays(GL_TRIANGLES, 0, 6); // invoke post process shader here!
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void Camera::PostProcessor::sampler(GLint binding, GLuint texture) {
	if(binding == 0) {
		printf("WARNING: In PostProcess shader, sampler binding 0 is reserved for world image.\n");
		return;
	}
	samplers_[binding] = texture;
}

void Camera::PostProcessor::uniform(const char* name, std::function<void(GLint)> callback) {
	GLint loc = glGetUniformLocation(shader_, name);
	if(loc == -1)
		printf("WARNING: Uniform \"%s\" not used in PostProcess shader.\n", name);
	uniform_update_functions_[loc] = callback;
}