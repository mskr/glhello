#ifndef POSTPROCESSOR_H_
#define POSTPROCESSOR_H_

#include <GL/glew.h>
#include <stdexcept>
#include "stdio.h"
#include "config.h"
#include "GPUBuffer.h"

/*
* The PostProcessor is for post processing effects.
* It uses a framebuffer with a texture attachment.
* The texture has the size of the viewport.
* Post processing has 2 shader passes:
* 1) Render world normally to texture.
* 2) Edit texture and render it on fullscreen quad.
* PostProcessors can be used by camera.
*/
class PostProcessor {
	GLuint framebuffer_;
	GLuint gpu_program_;
	GLuint texture_;
	GLuint vao_;
	static GLfloat fullscreen_quad_[];

public:
	PostProcessor();
	PostProcessor(GLuint gpu_program);
	~PostProcessor();

	void pass_1();
	void pass_2();
};

#endif