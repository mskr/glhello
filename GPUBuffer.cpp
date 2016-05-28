#include "GPUBuffer.h"

GPUBuffer::GPUBuffer() {
	glGenBuffers(1, &opengl_id_);
	bytes_ = 0;
}

GPUBuffer::~GPUBuffer() {
	// do not glDeleteBuffers here because
	// some GPUBuffer objects have local lifetime
}

void GPUBuffer::null() {
	glDeleteBuffers(1, &opengl_id_);
	opengl_id_ = 0;
	bytes_ = 0;
}

void GPUBuffer::bind_data(GLsizeiptr bytes, const GLvoid* pointer) {
	glBindBuffer(GL_ARRAY_BUFFER, opengl_id_);
	glBufferData(GL_ARRAY_BUFFER, bytes, pointer, GL_STATIC_DRAW);
	bytes_ = bytes;
}

void GPUBuffer::grow(GLsizeiptr incr_num) {
	bytes_ += incr_num;
}

void GPUBuffer::push(GLsizeiptr bytes_incr, const GLvoid* pointer) {
	if(bytes_ == 0) {
		bind_data(bytes_incr, pointer);
		return;
	}
	// resize by copy
	bind_to(GL_COPY_READ_BUFFER);
	GPUBuffer tmp_vbo;
	tmp_vbo.bind_to(GL_COPY_WRITE_BUFFER);
	glBufferData(GL_COPY_WRITE_BUFFER, bytes_+bytes_incr, 0, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, bytes_);
	glBufferSubData(GL_COPY_WRITE_BUFFER, bytes_, bytes_incr, pointer);
	bytes_ += bytes_incr;
	glDeleteBuffers(1, &opengl_id_);
	opengl_id_ = tmp_vbo.opengl_id();
}