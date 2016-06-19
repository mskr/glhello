#ifndef UNIFORM_H_
#define UNIFORM_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>

#include <functional>

#include "GPUBuffer.h"

class Model;

/*
* This is a uniform variable representation.
* It is the interface between client side and shader side.
* You can give a callback function to it.
* Uniforms can be (depending on what you use to construct them):
* a) simple: Pass a name and a callback, in which do uniform->update(_value_).
* b) a buffer: Pass a data pointer.
* Simple uniforms are updated per shader invocation.
* Buffers are updated
* a) only once, if you do not give a callback, or
* b) for every whole frame, if you do uniform->update() in callback.
* Buffered uniforms can be (depending again on constructor):
* a) Uniform blocks: Pass a data pointer and a _fixed_ size in bytes.
* b) Shader storage blocks: Only pass the pointer and do uniform->update_and_grow() in callback.
* In shaders, shader storage blocks can hold an array of undetermined size.
* You can fill and grow it only limited by GPU memory (you should store the current size somewhere).
*/
class Uniform {

	const GLchar* name_;
	GLuint gpu_program_;
	GLint location_;

	// simple uniforms use this callback (have to be updated for every shader invocation)
	std::function<void(Uniform*, Model*)> callback_; // called for every model

	// buffers use these members (update once and data remains in gpu memory)
	GPUBuffer* ubo_; // uniform buffer object
	const GLvoid* dataptr_; // data pointer
	static GLuint next_bindingpoint_; // binding point to link buffers to shader blocks
	GLuint bindingpoint_;
	std::function<void(Uniform*)> callback_whole_frame_; // called for every whole frame

public:

	// constructor for single value uniforms
	Uniform(const GLchar* name, std::function<void(Uniform*, Model*)> callback);

	// constructor for uniform buffers
	// Here the caller must take care that the pointer to data is still valid when this uniform is updated
	Uniform(const GLchar* name, const GLvoid* dataptr, GLsizeiptr size, std::function<void(Uniform*)> callback);

	// constructor for shader storage blocks (opengl 4.3+)
	// Here the caller must take care that the pointer to data is still valid when this uniform is updated
	// shader storage blocks are resizable
	Uniform(const GLchar* name, const GLvoid* dataptr, std::function<void(Uniform*)> callback);

	~Uniform();

	// Wrapper for the callback_ function.
	void callback(Model* m); // called by world object for the model currently rendered
	void callback(); // called by world for every whole frame

	// Must be called inside callback function.
	// Most visible function!
	// Each overload does an OpenGL call to the respective Uniform[type] function.
	void update(glm::mat4 val);
	void update(glm::vec3 val);
	void update(int val);
	void update(unsigned int val);
	void update(float val);
	// for uniform buffers
	void update();
	// for shader storage blocks (if bytes > 0, this operation is expensive!)
	// pointer points to the new data to push into the buffer, bytes is the new data's size
	void update_and_grow(const GLvoid* pointer, int bytes);

	// Setter for gpu program called by world object to complete this uniform.
	void gpu_program(GLuint gpu_program);

	// Getter
	int location() { return location_; }
	const GLchar* name() { return name_; }
	GLuint gpu_program() { return gpu_program_; }
	GPUBuffer* buffer() { return ubo_; }
	bool is_ubo() { return ubo_!=0; }
	bool has_bytes() { return ubo_->has_bytes(); }

};

#endif