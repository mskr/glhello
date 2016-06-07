#include "World.h"

#include "stdio.h"
#include "stdlib.h"

World::World(std::initializer_list<Model*> models, std::function<void(Model*)> draw_callback, std::initializer_list<Uniform> uniforms) {

	models_ = std::vector<Model*>(models);
	draw_callback_ = draw_callback;

	find_distinct_modeltypes(&modeltypes_); // fill modeltypes list
	find_distinct_gpu_programs(&gpu_programs_); // fill gpu programs list

	for(unsigned int i = 0; i < modeltypes_.size(); i++) {
		// for modeltype i: create vao (compare performance with a) 1 vao for each model, b) 1 vao for all models)
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		vertex_array_objects_.push_back(vao);

		// Send vertices to the GPU
		GPUBuffer vbo;
		vbo.bind();
		// for modeltype i: count bytes and reserve vertex memory
		for(Model* m : models_) if(m->modeltype() == modeltypes_[i]) 
			vbo.grow(m->bytes());
		glBufferData(GL_ARRAY_BUFFER, vbo.bytes(), 0, GL_STATIC_DRAW);
		// for modeltype i: initialize vertex memory
		GLintptr model_offset_bytes = 0;
		for(Model* m : models_) {
			if(m->modeltype() == modeltypes_[i]) {
				glBufferSubData(GL_ARRAY_BUFFER, model_offset_bytes, m->bytes(), m->pointer());
				model_offset_bytes += m->bytes();
			}
		}
		vertex_buffer_objects_.push_back(vbo);
		modeltypes_[i]->enable_attribs();

		// Send instanced arrays to GPU
		GPUBuffer instanced_array_buffer;
		instanced_array_buffer.bind();
		for(Model* m : models_) if(m->modeltype() == modeltypes_[i]) {
			instanced_array_buffer.grow(m->bytes_instance_attribs());
		}
		glBufferData(GL_ARRAY_BUFFER, instanced_array_buffer.bytes(), 0, GL_STATIC_DRAW);
		GLintptr o = 0;
		for(Model* m : models_) {
			if(m->modeltype() == modeltypes_[i]) {
				for(int j = 0; j < m->num_instances(); j++) {
					for(unsigned int k = 0; k < modeltypes_[i]->num_instance_attribs(); k++) {
						glBufferSubData(GL_ARRAY_BUFFER, o, modeltypes_[i]->instance_attr(k)->bytes(), m->pointer_instance_attr(j,k));
						o += modeltypes_[i]->instance_attr(k)->bytes();
					}
				}
			}
		}
		instanced_array_buffers_.push_back(instanced_array_buffer);
		modeltypes_[i]->enable_instance_attribs();
	}

	for(const Uniform &u : uniforms) {
		add(u);
	}
}

World::~World() {
	for(unsigned int i = 0; i < vertex_array_objects_.size(); i++)
		if (vertex_array_objects_[i] != 0) glDeleteVertexArrays(1, &vertex_array_objects_[i]);
	for(unsigned int i = 0; i < uniform_buffers_.size(); i++)
		delete uniform_buffers_[i].buffer();
}



//TODO make models and instances not just addable but removable too!

void World::draw() {
	for(unsigned int j = 0; j < gpu_programs_.size(); j++) {
		glUseProgram(gpu_programs_[j]);

		for(Uniform &uniform : uniform_buffers_)
			if(uniform.gpu_program() == gpu_programs_[j]) uniform.callback();
		for(unsigned int i = 0; i < modeltypes_.size(); i++) {
			if(modeltypes_[i]->gpu_program() == gpu_programs_[j]) {
				glBindVertexArray(vertex_array_objects_[i]);

				GLint vertices_offset = 0;
				GLint instance_attribs_offset = 0;
				for(Model* m : models_) {
					if(m->modeltype() == modeltypes_[i]) {
						
						m->update_instance_attribs(&instanced_array_buffers_[i], instance_attribs_offset);
						//TODO check for new instances and add them into buffer

						for(Uniform &uniform : uniforms_) // call the Uniform update callback
							if(uniform.gpu_program() == gpu_programs_[j]) uniform.callback(m);

						draw_callback_(m);

						m->draw(vertices_offset, instance_attribs_offset); // Shader invocation here

						vertices_offset += m->num_vertices();
						instance_attribs_offset += m->num_instances();
					}
				}
			}
		}
	}
}

void World::add(Module* mod) {
	for(Uniform u : mod->uniforms()) add(u);
}

void World::add(Uniform u) {
	// for each gpu program: *copy* uniform to list
	// => the same uniform exists for each gpu program
	for(GLuint gpu_program : gpu_programs_) {
		if(u.is_ubo() && u.has_bytes()) {
			u.gpu_program(gpu_program);
			uniform_buffers_.push_back(u);
		} else if(u.is_ubo()) {
			// Uniform is a shader storage buffer
			u.gpu_program(gpu_program);
			uniform_buffers_.push_back(u);
		} else {
			u.gpu_program(gpu_program);
			uniforms_.push_back(u);
		}
	}
}

//TODO rewrite for instance attributes
//TODO Test!
void World::add(Model* m) {
	ModelType* t = m->modeltype();
	bool is_of_new_type = true;
	unsigned int i;
	for(i = 0; i < modeltypes_.size(); i++) {
		if(modeltypes_[i] == t) {
			is_of_new_type = false;
			break;
		}
	}
	if(is_of_new_type) {
		// create new vbo
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		vertex_array_objects_.push_back(vao);
		GPUBuffer vbo;
		vbo.bind_data(m->bytes(), m->pointer());
		vertex_buffer_objects_.push_back(vbo);
		t->enable_attribs();
		GPUBuffer modelmatrix_buffer;
		modelmatrix_buffer.bind_data(m->bytes_matrices(), m->pointer_matrices());
		GLuint gpu_prog = t->gpu_program();
		bool has_new_gpuprogram = !(std::find(gpu_programs_.begin(), gpu_programs_.end(), gpu_prog) == gpu_programs_.end());
		if(has_new_gpuprogram) {
			// create copies of uniforms
			for(Uniform u : uniforms_) {
				u.gpu_program(gpu_prog);
				uniforms_.push_back(u);
			}
			for(Uniform u : uniform_buffers_) {
				u.gpu_program(gpu_prog);
				uniform_buffers_.push_back(u);
			}
			gpu_programs_.push_back(gpu_prog);
		}
	} else {
		// resize old vbo
		glBindVertexArray(vertex_array_objects_[i]);
		vertex_buffer_objects_[i].push(m->bytes(), m->pointer());
	}
	models_.push_back(m);
}


void World::find_distinct_modeltypes(std::vector<ModelType*>* v) {
	for(Model* model : models_) {
		if(std::find(v->begin(), v->end(), model->modeltype()) == v->end())
			v->push_back(model->modeltype());
	}
}


void World::find_distinct_gpu_programs(std::vector<GLuint>* v) {
	for(ModelType* modeltype : modeltypes_) {
		if(std::find(v->begin(), v->end(), modeltype->gpu_program()) == v->end())
			v->push_back(modeltype->gpu_program());
	}
}