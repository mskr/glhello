#ifndef GPUBUFFER_H_
#define GPUBUFFER_H_

#include <GL/glew.h>

#include <initializer_list>
#include <tuple>
#include "stdio.h"

/*
* A gpu buffer representation.
* Can be used as
* a) Vertex buffer
* b) Framebuffer
* This representation holds no data itself.
* The data lies in GPU memory.
* This representation only holds the size.
* Change size with grow().
* Set data with glBuffer<Sub>Data()...
* but remember to bind() the buffer before.
* The default binding point is GL_ARRAY_BUFFER.
* WARNING:
* It is your task to keep this buffer representation consistent.
* Call grow() after setting data.
* True number of bytes is returned by glGetBufferParameteriv with GL_BUFFER_SIZE.
*/
class GPUBuffer {

	bool is_fbo_;

	GLuint opengl_id_;
	GLsizeiptr bytes_;

public:
	// Create multipurpose buffer
	GPUBuffer(GLsizeiptr bytes);
	GPUBuffer(GLsizeiptr bytes, const GLvoid* pointer);
	// Create framebuffer with one or more 2D textures
	// Each tuple must contain: pointer to texture id, attachment point, texture storage format and datatype
	//TODO Separate Framebuffer class with add(Texture) function etc.
	GPUBuffer(GLsizei w, GLsizei h, std::initializer_list<std::tuple<GLuint*,GLenum,GLint,GLenum>> tex_attachments);
	~GPUBuffer();

	// Delete buffer
	void null();

	GLuint opengl_id() { return opengl_id_; }
	GLsizeiptr bytes() { return bytes_; }
	bool has_bytes() { return (bytes_>0); }
	void grow(GLsizeiptr incr_num);

	void bind();
	void bind_to(GLenum targetpoint);
	void bind_data(GLsizeiptr bytes, const GLvoid* pointer); // bind and init gpu memory

	// Resize buffer by copy contents from one VBO to another.
	void push(GLsizeiptr bytes_incr, const GLvoid* pointer);

	// Create empty 2D texture without mipmap
	static GLuint new_texture2D(GLsizei w, GLsizei h, GLint format, GLenum type);
	
};

#endif