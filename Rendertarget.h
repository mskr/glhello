#ifndef RENDERTARGET_H_
#define RENDERTARGET_H_

class Rendertarget {

	GLuint opengl_id_;

public:
	Rendertarget() {
		glGenFramebuffers(1, &opengl_id_);
	}

	~Rendertarget();

	void bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, opengl_id_);
	}

};

#endif