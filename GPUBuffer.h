#ifndef GPUBUFFER_H_
#define GPUBUFFER_H_

#include <GL/glew.h>

/*
* A gpu buffer representation.
* Can be used as vertex buffer.
* This buffer holds no data itself.
* The data lies in GPU memory.
* This representation only holds the size.
* Change size with grow().
* Set data with glBuffer<Sub>Data()...
* but remember to bind() the buffer before.
* Shortcut for bind and set data is bind_data().
* The default binding point is GL_ARRAY_BUFFER.
* WARNING:
* It is your task to keep this buffer representation consistent.
* Call grow() after setting data.
* True number of bytes is returned by glGetBufferParameteriv with GL_BUFFER_SIZE.
*/
class GPUBuffer {

	GLuint opengl_id_;
	GLsizeiptr bytes_;

public:
	GPUBuffer();
	~GPUBuffer();

	void null();

	GLuint opengl_id() { return opengl_id_; }
	GLsizeiptr bytes() { return bytes_; }
	bool has_bytes() { return (bytes_>0); }
	void grow(GLsizeiptr incr_num);

	void bind() { glBindBuffer(GL_ARRAY_BUFFER, opengl_id_); }
	void bind_to(GLenum targetpoint) { glBindBuffer(targetpoint, opengl_id_); }
	void bind_data(GLsizeiptr bytes, const GLvoid* pointer); // bind and init gpu memory

	// Resize buffer by copy contents from one VBO to another.
	void push(GLsizeiptr bytes_incr, const GLvoid* pointer);
	
};

#endif