#include "World.h"

#include "stdio.h"
#include "stdlib.h"

World::World(std::initializer_list<Model*> models, std::function<void(Model*)> draw_callback, std::initializer_list<Uniform> uniforms) {

	models_ = std::vector<Model*>(models);
	draw_callback_ = draw_callback;

	find_distinct_modeltypes(&modeltypes_); // Fill modeltypes list
	find_distinct_gpu_programs(&gpu_programs_); // Fill gpu programs list
	bind_shader_locations(&vertex_attribs_, &instance_attribs_); // Fill attrib lists

	// Create and fill buffers
	for(unsigned int i = 0; i < modeltypes_.size(); i++) {
		// for modeltype i: create vao (compare performance with a) 1 vao for each model, b) 1 vao for all models)
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		vertex_array_objects_.push_back(vao);

		// Send vertices to the GPU
		GPUBuffer vbo(0);
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
		modeltypes_[i]->format_vertex_buffer();

		// Send instanced arrays to GPU
		GPUBuffer instanced_array_buffer(0);
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
		modeltypes_[i]->format_instance_buffer();
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


void World::draw(GLuint rendertarget) {

	draw_modules();

	glBindFramebuffer(GL_FRAMEBUFFER, rendertarget);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(unsigned int j = 0; j < gpu_programs_.size(); j++) {
		glUseProgram(gpu_programs_[j]);

		// Update uniform buffers
		for(Uniform &uniform : uniform_buffers_)
			if(uniform.gpu_program() == gpu_programs_[j])
				uniform.callback();

		//TODO display FPS
		for(unsigned int i = 0; i < modeltypes_.size(); i++) {
			if(modeltypes_[i]->gpu_program() == gpu_programs_[j]) {
				glBindVertexArray(vertex_array_objects_[i]);

				GLint vertices_offset = 0;
				GLint instance_attribs_offset = 0;
				for(Model* m : models_) {
					if(m->modeltype() == modeltypes_[i]) {
						
						// Update model matrix and other instance attributes
						m->update_instance_attribs(&instanced_array_buffers_[i], instance_attribs_offset);
						//TODO check for new instances and add them into buffer

						// Update simple uniforms
						for(Uniform &uniform : uniforms_)
							if(uniform.gpu_program() == gpu_programs_[j])
								uniform.callback(m);

						draw_callback_(m);

						// Render part of the buffer (TODO try render everthing at once)
						m->draw(vertices_offset, instance_attribs_offset);

						vertices_offset += m->num_vertices();
						instance_attribs_offset += m->num_instances();
					}
				}
			}
		}
	}
}

void World::draw_modules() {
	for(Module* mod : modules_) {
		if(mod->shader() == 0) continue;
		glUseProgram(mod->shader());
		for(int i = 0; i < mod->num_passes(); i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, mod->rendertarget(i));
			mod->on_pass(i);
			mod->update(); // Tell module it's time to update uniform buffers

			for(unsigned int j = 0; j < modeltypes_.size(); j++) {
				glBindVertexArray(vertex_array_objects_[j]);

				GLint vertices_offset = 0;
				GLint instance_attribs_offset = 0;
				for(Model* m : models_) {
					if(m->modeltype() == modeltypes_[j]) {
						mod->update(m); // Tell module it's time to update simple uniforms
						m->draw(vertices_offset, instance_attribs_offset);
						vertices_offset += m->num_vertices();
						instance_attribs_offset += m->num_instances();
					}
				}
			}
			mod->after_pass(i);
		}
	}
}

void World::extend(Module* mod) {
	// Copy uniforms FROM this module
	for(Uniform u : mod->uniforms())
		add(u);
	// Bind data to the module shader
	if(mod->shader() != 0) {
		// Copy uniforms (that yet exist) TO module
		for(Uniform u : uniform_buffers_) {
			mod->add(u);
		}
		for(Uniform u : uniforms_) {
			mod->add(u);
		}
		// Make vertex attributes available
		for(GLuint location = 0; location < vertex_attribs_.size(); location++) {
			vertex_attribs_[location]->bind_location(mod->shader(), location);
		}
		// Make instance attributes available
		for(GLuint location = 0; location < instance_attribs_.size(); location++) {
			if(instance_attribs_[location] == 0) continue;
			instance_attribs_[location]->bind_locations(mod->shader(), location);
		}
		glLinkProgram(mod->shader());
	}
	modules_.push_back(mod);
}

void World::add(Uniform u) {
	// For each gpu program, make a *copy* of the uniform.
	// The uniform class checks whether it is defined in gpu program.
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
	// Copy the new uniform to modules that yet exist
	for(Module* mod : modules_)
		if(mod->shader() != 0)
			mod->add(u);
}

//TODO adding models at runtime: rewrite and test!
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
		GPUBuffer vbo(0);
		vbo.bind_data(m->bytes(), m->pointer());
		vertex_buffer_objects_.push_back(vbo);
		t->format_vertex_buffer();
		GPUBuffer modelmatrix_buffer(0);
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


void World::bind_shader_locations(std::vector<VertexAttribute*>* bound_vattribs, std::vector<InstanceAttribute*>* bound_iattribs) {
	// Bind attributes following the rule: equal name(s) <=> equal location
	for(ModelType* modeltype : modeltypes_) {
		// First bind vertex attribs
		for(unsigned int i = 0; i < modeltype->num_vertex_attribs(); i++) {
			// Search for vattr i under the bound vattribs
			VertexAttribute* vattr = modeltype->vertex_attr(i);
			bool found = false;
			for(unsigned int loc = 0; loc < bound_vattribs->size(); loc++) {
				if(vattr->equals(bound_vattribs->at(loc))) {
					// Found attr with same name at loc
					modeltype->bind_vertex_attr_location(i, loc);
					found = true;
					break;
				}
			}
			if(found) continue;
			modeltype->bind_vertex_attr_location(i, bound_vattribs->size());
			bound_vattribs->push_back(vattr);
		}
	}
	// Resize list so that locations of vertex- and instance-attribs do not overlap
	bound_iattribs->resize(bound_vattribs->size(), 0);
	for(ModelType* modeltype : modeltypes_) {
		// Now bind instance attribs
		for(unsigned int i = 0; i < modeltype->num_instance_attribs(); i++) {
			InstanceAttribute* iattr = modeltype->instance_attr(i);
			bool found = false;
			for(unsigned int start_loc = 0; start_loc < bound_iattribs->size(); start_loc++) {
				if(iattr->equals(bound_iattribs->at(start_loc))) {
					modeltype->bind_instance_attr_locations(i, start_loc);
					found = true;
					break;
				}
			}
			if(found) continue;
			int num_locs = modeltype->bind_instance_attr_locations(i, bound_iattribs->size());
			bound_iattribs->push_back(iattr);
			bound_iattribs->resize(bound_iattribs->size() + num_locs - 1, 0);
		}
	}
	for(ModelType* modeltype : modeltypes_)
		modeltype->link_gpu_program();
}