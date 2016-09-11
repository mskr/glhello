#include "Uniform.h"

#include "stdio.h"
#include "stdlib.h"

GLuint Uniform::next_bindingpoint_ = 0;

Uniform::Uniform(const GLchar* name, std::function<void(Uniform*, Model*)> callback) {
	name_ = name;
	callback_ = callback;
	callback_whole_frame_ = 0;
	gpu_program_ = -1;
	location_ = -1;
	ubo_ = 0;
	dataptr_ = 0;
	bindingpoint_ = GL_MAX_UNIFORM_BUFFER_BINDINGS;
}

Uniform::Uniform(const GLchar* name, const GLvoid* dataptr, GLsizeiptr size, std::function<void(Uniform*)> callback) {
	name_ = name;
	callback_ = 0;
	callback_whole_frame_ = callback;
	gpu_program_ = -1;
	location_ = -1;
	dataptr_ = dataptr;
	ubo_ = new GPUBuffer(0);
	ubo_->bind_to(GL_UNIFORM_BUFFER);
	glBufferData(GL_UNIFORM_BUFFER, size, dataptr_, GL_STATIC_DRAW);
	ubo_->grow(size);
	bindingpoint_ = GL_MAX_UNIFORM_BUFFER_BINDINGS;
}

Uniform::Uniform(const GLchar* name, const GLvoid* dataptr, std::function<void(Uniform*)> callback) {
	name_ = name;
	callback_ = 0;
	callback_whole_frame_ = callback;
	gpu_program_ = -1;
	location_ = -1;
	dataptr_ = dataptr;
	// use UBO as a shader storage buffer (some people call it a Shader Storage Buffer Object then)
	ubo_ = new GPUBuffer(0);
	ubo_->bind_to(GL_SHADER_STORAGE_BUFFER);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, dataptr_, GL_STATIC_DRAW);
	bindingpoint_ = GL_MAX_UNIFORM_BUFFER_BINDINGS;
}

Uniform::~Uniform() {
	// do not worry about ubo_ pointer (it is cleaned by world)
}

void Uniform::gpu_program(GLuint gpu_program) {
	gpu_program_ = gpu_program;
	// Get the shader variable and save its GPU-memory-location
	if(is_ubo()) {
		if(Uniform::next_bindingpoint_ == GL_MAX_UNIFORM_BUFFER_BINDINGS) return;
		if(ubo_->has_bytes()) {
			// This is a Uniform Buffer Object
			// connect client-side ubo and shader-side uniform block via one binding point
			GLuint index = glGetUniformBlockIndex(gpu_program_, name_);
			if(index == GL_INVALID_INDEX) {
				printf("WARNING: Uniform \"%s\" not used in shader#%d.\n", name_, gpu_program);
				return;
			}
			glUniformBlockBinding(gpu_program_, index, Uniform::next_bindingpoint_);
			glBindBufferBase(GL_UNIFORM_BUFFER, Uniform::next_bindingpoint_, ubo_->opengl_id());
			bindingpoint_ = Uniform::next_bindingpoint_;
			Uniform::next_bindingpoint_++;
		} else {
			// This is a Shader Storage Buffer Object
			// connect shader-storage-buffer and shader-storage-block
			GLuint index = glGetProgramResourceIndex(gpu_program_, GL_SHADER_STORAGE_BLOCK, name_);
			if(index == GL_INVALID_INDEX) {
				printf("WARNING: Uniform \"%s\" not used in shader#%d.\n", name_, gpu_program);
				return;
			}
			glShaderStorageBlockBinding(gpu_program_, index, Uniform::next_bindingpoint_);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Uniform::next_bindingpoint_, ubo_->opengl_id());
			bindingpoint_ = Uniform::next_bindingpoint_;
			Uniform::next_bindingpoint_++;
		}
	} else { // This is for simple Uniform variables
		location_ = glGetUniformLocation(gpu_program_, name_);
		if(location_ == -1) printf("WARNING: Uniform \"%s\" not used in shader#%d.\n", name_, gpu_program);
	}
}

void Uniform::callback(Model* m) {
	if(location_ == -1 || callback_ == 0) return;
	else callback_(this, m);
}

void Uniform::callback() {
	if(callback_whole_frame_ == 0) return;
	else callback_whole_frame_(this);
}

void Uniform::update(glm::mat4 val) { glUniformMatrix4fv(location_, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&val)); }
void Uniform::update(glm::vec3 val) { glUniform3f(location_, val.x, val.y, val.z); }
void Uniform::update(int val) { glUniform1i(location_, val); }
void Uniform::update(unsigned int val) { glUniform1ui(location_, val); }
void Uniform::update(float val) { glUniform1f(location_, val); }
void Uniform::update() {
	// should never be called by simple uniform callbacks because ubo_ would point to uninit memory (segfault)
	ubo_->bind_to(GL_UNIFORM_BUFFER);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, ubo_->bytes(), dataptr_); // update whole buffer data
}
void Uniform::update_and_grow(const GLvoid* pointer, int bytes) {
	// should never be called by simple uniform callbacks because ubo_ would point to uninit memory (segfault)
	// Beware, pointer just points to the NEW data
	// Only pushing new data if bytes is greater zero
	// Updating the old data (starting at dataptr_) in both cases
	if(bindingpoint_ == GL_MAX_UNIFORM_BUFFER_BINDINGS) return;
	// Is there something to update?
	if(ubo_->bytes() > 0) {
		ubo_->bind_to(GL_SHADER_STORAGE_BUFFER);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ubo_->bytes(), dataptr_); // update whole OLD buffer data
	}
	// Is there something NEW to update?
	if(bytes > 0) {
		ubo_->push(bytes, pointer); // expensive operation (re-allocation of gpu memory)
		ubo_->bind_to(GL_SHADER_STORAGE_BUFFER);
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, bindingpoint_, ubo_->opengl_id(), 0, ubo_->bytes());
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}