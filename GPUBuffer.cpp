#include "GPUBuffer.h"

GPUBuffer::GPUBuffer(GLsizeiptr bytes) {
	glGenBuffers(1, &opengl_id_);
	bytes_ = bytes;

	is_fbo_ = false; //TODO ugly
}

GPUBuffer::GPUBuffer(GLsizeiptr bytes, const GLvoid* pointer) {
	glGenBuffers(1, &opengl_id_);
	glBindBuffer(GL_ARRAY_BUFFER, opengl_id_);
	glBufferData(GL_ARRAY_BUFFER, bytes, pointer, GL_STATIC_DRAW);
	bytes_ = bytes;

	is_fbo_ = false;
}

GPUBuffer::GPUBuffer(GLsizei w, GLsizei h, std::initializer_list<std::tuple<GLuint*,GLenum,GLint,GLenum>> tex_attachments) {
	glGenFramebuffers(1, &opengl_id_);
	glBindFramebuffer(GL_FRAMEBUFFER, opengl_id_);
	int num_color_attachments = 0;
	int num_depth_attachments = 0;
	int num_stencil_attachments = 0;
	for(std::tuple<GLuint*,GLenum,GLint,GLenum> t : tex_attachments) {
		GLuint* tex_ptr = std::get<0>(t);
		*tex_ptr = new_texture2D(w, h, std::get<2>(t), std::get<3>(t));
		GLenum attachment_type = std::get<1>(t);
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D, *tex_ptr, 0);
		// Count attachments
		GLint max_color_attachments = 8;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
		if((int)attachment_type >= (int)GL_COLOR_ATTACHMENT0 && (int)attachment_type <= ((int)GL_COLOR_ATTACHMENT0 + max_color_attachments - 1))
			num_color_attachments++;
		else if(attachment_type == GL_DEPTH_ATTACHMENT)
			num_depth_attachments++;
		else if(attachment_type == GL_STENCIL_ATTACHMENT)
			num_stencil_attachments++;
		else if(attachment_type == GL_DEPTH_STENCIL_ATTACHMENT) {
			num_depth_attachments++;
			num_stencil_attachments++;
		}
	}
	// If no color texture, disable color rendering
	if(num_color_attachments == 0) {
		glDrawBuffer(GL_NONE); // OpenGL won't try to write to a color buffer
		glReadBuffer(GL_NONE); // reading from color buffer is also disabled
	} else if(num_color_attachments > 1) {
		//TODO Link fragment shader out variables to attachments with glBindFragDataLocation
	}
	// If no depth and stencil texture, attach renderbuffer
	if(num_depth_attachments == 0 && num_stencil_attachments == 0) {
		// Renderbuffer is write-only, and probably faster than textures
		GLuint renderbuffer = 0;
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
	} else if(num_depth_attachments == 0) {
		//TODO do something for color+stencil attachments
	} else if(num_stencil_attachments == 0) {
		//TODO do something for color+depth attachments
	}
	// Check if framebuffer complete
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Program exits because I failed to complete framebuffer.");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	bytes_ = 0; //TODO calculate frambuffer byte size

	is_fbo_ = true;
}

GPUBuffer::~GPUBuffer() {
	// do not glDeleteBuffers here because
	// some GPUBuffer objects have local lifetime
}

void GPUBuffer::bind() {
	if(is_fbo_)
		glBindFramebuffer(GL_FRAMEBUFFER, opengl_id_);
	else
		glBindBuffer(GL_ARRAY_BUFFER, opengl_id_);
}

void GPUBuffer::bind_to(GLenum targetpoint) {
	if(is_fbo_)
		glBindFramebuffer(targetpoint, opengl_id_);
	else
		glBindBuffer(targetpoint, opengl_id_);
}

void GPUBuffer::null() {
	if(is_fbo_)
		glDeleteFramebuffers(1, &opengl_id_);
	else
		glDeleteBuffers(1, &opengl_id_);
	opengl_id_ = 0;
	bytes_ = 0;
}

void GPUBuffer::bind_data(GLsizeiptr bytes, const GLvoid* pointer) {
	if(is_fbo_)
		return;
	glBindBuffer(GL_ARRAY_BUFFER, opengl_id_);
	glBufferData(GL_ARRAY_BUFFER, bytes, pointer, GL_STATIC_DRAW);
	bytes_ = bytes;
}

void GPUBuffer::grow(GLsizeiptr incr_num) {
	bytes_ += incr_num;
}

void GPUBuffer::push(GLsizeiptr bytes_incr, const GLvoid* pointer) {
	if(is_fbo_)
		return;
	if(bytes_ == 0) {
		bind_data(bytes_incr, pointer);
		return;
	}
	// resize by copy
	bind_to(GL_COPY_READ_BUFFER);
	GPUBuffer tmp_vbo(0);
	tmp_vbo.bind_to(GL_COPY_WRITE_BUFFER);
	glBufferData(GL_COPY_WRITE_BUFFER, bytes_+bytes_incr, 0, GL_STATIC_DRAW);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, bytes_);
	glBufferSubData(GL_COPY_WRITE_BUFFER, bytes_, bytes_incr, pointer);
	bytes_ += bytes_incr;
	glDeleteBuffers(1, &opengl_id_);
	opengl_id_ = tmp_vbo.opengl_id();
}

GLuint GPUBuffer::new_texture2D(GLsizei w, GLsizei h, GLint format, GLenum type) {
	GLuint tex;
	glGenTextures(1, &tex),
	glBindTexture(GL_TEXTURE_2D, tex);
	// Only accept writing to texture with the data formats
	// GL_RED, GL_RG, GL_RGB, GL_RGBA, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT and GL_DEPTH_STENCIL.
	//TODO Better use sized formats like GL_R8, GL_RG8, GL_RGB8 etc.
	GLenum write_format = GL_RGB;
	// The format argument specifies the data that is stored at each texel
	if(format == GL_STENCIL_INDEX || format == GL_DEPTH_COMPONENT || format == GL_DEPTH_STENCIL)
		write_format = format;
	// Adapt the write format to possible high resolution data, > 8 bit per component
	else if(format == GL_RGB16F || format == GL_RGB32F)
		write_format = GL_RGB;
	else if(format == GL_RGBA || format == GL_RGBA16F || format == GL_RGBA32F)
		write_format = GL_RGBA;
	else if(format == GL_RG || format == GL_RG16F || format == GL_RG32F)
		write_format = GL_RG;
	else if(format == GL_RED || format == GL_R16F || format == GL_R32F)
		write_format = GL_RED;
	else //Default is GL_RGB for store and write
		format = write_format;
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, write_format, type, (GLvoid*)0);
	return tex;
}