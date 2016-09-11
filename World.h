#ifndef WORLD_H_
#define WORLD_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtc/matrix_access.hpp>
#include <gtc/quaternion.hpp>

#include <vector>
#include <functional>
#include <algorithm>

#include "Module.h"
#include "Model.h"
#include "Uniform.h"
#include "GPUBuffer.h"

/*
* This is the world.
* It is a space where you can place an infinite amount of models.
* Throw models into the world using
* a) the list given to the world constructor or
* b) the add() method.
* Adding models to the world makes "mat4 model" available in shaders.
* Use the model matrices to place models at their location in the world.
* You can also give uniform variables to the world using
* a) the list given to the world constructor or
* b) the add() method.
* The uniforms will be available in all shaders.
* You can also give a callback function to the world constructor.
* The callback will be called just before shader invocation (i.e. for each model getting rendered).
*/
class World {

	// Holds pointers to model objects residing in this world
	std::vector<Model*> models_;
	// Holds pointers to all *distinct* model types
	std::vector<ModelType*> modeltypes_;
	// Holds vertex array objects, one for each model type
	std::vector<GLuint> vertex_array_objects_;
	// Holds vertex buffer objects, one for each model type
	std::vector<GPUBuffer> vertex_buffer_objects_;
	// Holds instanced array buffers, one for each model type
	std::vector<GPUBuffer> instanced_array_buffers_;
	// Holds all *distinct* gpu programs
	std::vector<GLuint> gpu_programs_;
	// Holds pointers to the distinct vertex attributes of all gpu programs
	std::vector<VertexAttribute*> vertex_attribs_; // *Index == attrib location*
	// Holds pointers to the distinct instance attributes of all gpu programs
	std::vector<InstanceAttribute*> instance_attribs_; // *Index == attrib location*
	// Holds all modules
	std::vector<Module*> modules_;

	// Private utility function that uses the models list,
	// finds all distinct model types and stores them in the modeltypes list
	void find_distinct_modeltypes(std::vector<ModelType*>* v);

	// Private utility function that uses the modeltypes list,
	// finds all distinct gpu programs and stores them in the given list
	void find_distinct_gpu_programs(std::vector<GLuint>* v);

	// Private utility function that uses the modeltypes list,
	// finds distinct attributes in all gpu programs,
	// binds them so that equal attributes have equal locations
	// and so that the list indices are equal to locations.
	void bind_shader_locations(std::vector<VertexAttribute*>* vattribs, std::vector<InstanceAttribute*>* iattribs);

	// Tell the GPU how model matrices are stored and connected to shader variables
	void set_modelmatrix_memory_layout(GPUBuffer* buffer, int offset, GLuint gpu_program);
	// Check if instances have been transformed and if so update the buffer
	void update_modelmatrices(Model* model, GPUBuffer* buffer, GLintptr offset);
	// Check if there are new instances and if so resize the buffer
	void update_instance_count(Model* model, GPUBuffer* buffer);

	// Holds the callback that is called in draw loop for each model
	std::function<void(Model*)> draw_callback_; //TODO remove?

	// Holds the uniforms that are updated for each model
	std::vector<Uniform> uniforms_;
	
	// Holds the uniforms that are updated for each complete frame
	std::vector<Uniform> uniform_buffers_;
	

	//TODO add time to the world
	//Clock c; // clock can (1) save a time in the past, (2) return current time, (3) return delta time


public:
	World(
		std::initializer_list<Model*> models, 
		std::function<void(Model*)> draw_callback, 
		std::initializer_list<Uniform> uniforms);
	~World();

	void draw(GLuint rendertarget);

	void draw_modules();

	void add(Uniform u);
	void add(Model* m); //TODO make models and instances not just addable but removable too!
	void extend(Module* mod);

	//TODO

	//void step(long delta_time); // for time-dependent/animated drawing (pass delta_time to animation modules)
};


#endif