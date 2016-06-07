#include "PostProcessor.h"

GLfloat PostProcessor::fullscreen_quad_[] = {
	// positions   // tex coords
	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f,  1.0f,  1.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f
};

PostProcessor::PostProcessor() {
	framebuffer_ = 0;
	gpu_program_ = 0;
	texture_ = 0;
	vao_ = 0;
}

PostProcessor::PostProcessor(GLuint gpu_program) {
	gpu_program_ = gpu_program;
	glGenFramebuffers(1, &framebuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	// Allocate GPU memory for color, depth and stencil values
	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		config::viewport_width, config::viewport_height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		(GLvoid*)0 // this texture is not initialized with data
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	GLuint renderbuffer = 0;
	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
		config::viewport_width, config::viewport_height
	);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Color ~> Texture (read- and write-access in shaders!)
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_, 0);
	// Depth and Stencilvalues ~> Renderbuffer (write-only but faster)
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Program exits because PostProcessor failed to complete framebuffer.");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // makes the default framebuffer active again

	// Upload vertices of fullscreen quad
	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);
	GPUBuffer vbo;
	vbo.bind_data(sizeof(fullscreen_quad_), fullscreen_quad_);
	GLint loc1 = glGetAttribLocation(gpu_program_, "position");
	GLint loc2 = glGetAttribLocation(gpu_program_, "texCoords");
	glVertexAttribPointer(loc1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(loc1);
	glEnableVertexAttribArray(loc2);
	glBindVertexArray(0);
}

PostProcessor::~PostProcessor() {

}

void PostProcessor::pass_1() {
	if(framebuffer_ == 0 && gpu_program_ == 0) return;
	// render world to texture
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
	glEnable(GL_DEPTH_TEST);
}

void PostProcessor::pass_2() {
	if(framebuffer_ == 0 && gpu_program_ == 0) return;
	// render textured quad to default framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT); // no need for depth clearing as quad has no depth anyway
	glUseProgram(gpu_program_);
	glBindVertexArray(vao_);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}