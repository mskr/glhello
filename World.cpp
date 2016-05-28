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
		for(Model* m : models_) if(m->modeltype() == modeltypes_[i]) vbo.grow(m->bytes());
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
		

		// Send matrices to the GPU
		GPUBuffer modelmatrix_buffer;
		modelmatrix_buffer.bind();
		for(Model* m : models_) if(m->modeltype() == modeltypes_[i]) modelmatrix_buffer.grow(m->bytes_matrices());
		glBufferData(GL_ARRAY_BUFFER, modelmatrix_buffer.bytes(), 0, GL_STATIC_DRAW);
		GLintptr matrix_offset_bytes = 0;
		for(Model* m : models_) {
			if(m->modeltype() == modeltypes_[i]) {
				glBufferSubData(GL_ARRAY_BUFFER, matrix_offset_bytes, m->bytes_matrices(), m->pointer_matrices());
				matrix_offset_bytes += m->bytes_matrices();
			}
		}
		modelmatrix_buffers_.push_back(modelmatrix_buffer);
	}

	add(Uniform("ambient_light", [](Uniform* u, Model* m) {
		u->update(0.1f);
	}));

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
				GLint matrices_offset = 0;
				for(Model* m : models_) {
					if(m->modeltype() == modeltypes_[i]) {

						update_modelmatrices(m, &modelmatrix_buffers_[i], matrices_offset);
						update_instance_count(m, &modelmatrix_buffers_[i]);
						set_modelmatrix_memory_layout(&modelmatrix_buffers_[i], matrices_offset, gpu_programs_[j]);

						for(Uniform &uniform : uniforms_) // call the Uniform update callback
							if(uniform.gpu_program() == gpu_programs_[j]) uniform.callback(m);

						draw_callback_(m);

						m->draw(vertices_offset);
						vertices_offset += m->num_vertices();
						matrices_offset += m->num_matrices();
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


void World::set_modelmatrix_memory_layout(GPUBuffer* buffer, int offset, GLuint gpu_program) {
	// Using an instanced array (vertex attributes that remain for 1 instance)
	buffer->bind();
	GLsizei matrix_bytes = 4*4 * sizeof(GLfloat);
	const GLvoid* column_1_offset = (GLvoid*)(offset * matrix_bytes + 0);
	const GLvoid* column_2_offset = (GLvoid*)(offset * matrix_bytes + 4 * sizeof(GLfloat));
	const GLvoid* column_3_offset = (GLvoid*)(offset * matrix_bytes + 8 * sizeof(GLfloat));
	const GLvoid* column_4_offset = (GLvoid*)(offset * matrix_bytes + 12 * sizeof(GLfloat));
	GLuint location_1 = glGetAttribLocation(gpu_program, "model_matrix_column_1");
	glEnableVertexAttribArray(location_1);
	glVertexAttribPointer(location_1, 4, GL_FLOAT, GL_FALSE, matrix_bytes, column_1_offset);
	glVertexAttribDivisor(location_1, 1); // get next attribute from buffer after 1 instance
	GLuint location_2 = glGetAttribLocation(gpu_program, "model_matrix_column_2");
	glEnableVertexAttribArray(location_2);
	glVertexAttribPointer(location_2, 4, GL_FLOAT, GL_FALSE, matrix_bytes, column_2_offset);
	glVertexAttribDivisor(location_2, 1); // get next attribute from buffer after 1 instance
	GLuint location_3 = glGetAttribLocation(gpu_program, "model_matrix_column_3");
	glEnableVertexAttribArray(location_3);
	glVertexAttribPointer(location_3, 4, GL_FLOAT, GL_FALSE, matrix_bytes, column_3_offset);
	glVertexAttribDivisor(location_3, 1); // get next attribute from buffer after 1 instance
	GLuint location_4 = glGetAttribLocation(gpu_program, "model_matrix_column_4");
	glEnableVertexAttribArray(location_4);
	glVertexAttribPointer(location_4, 4, GL_FLOAT, GL_FALSE, matrix_bytes, column_4_offset);
	glVertexAttribDivisor(location_4, 1); // get next attribute from buffer after 1 instance
}


void World::update_modelmatrices(Model* model, GPUBuffer* buffer, GLintptr offset) {
	buffer->bind();
	for(int i = 0; i < model->num_instances(); i++) {
		ModelInstance* instance = model->instance(i);
		if(instance->has_changed()) {
			glBufferSubData(GL_ARRAY_BUFFER, 
				offset * sizeof(glm::mat4) + i * sizeof(glm::mat4), 
				sizeof(glm::mat4), 
				instance->model_matrix());
			instance->was_updated();
		}
	}
}


void World::update_instance_count(Model* model, GPUBuffer* buffer) {
	int num_new_inst = model->num_new_instances();
	if(num_new_inst > 0) {
		buffer->push(
			num_new_inst * sizeof(glm::mat4),
			model->instance(model->num_instances()-num_new_inst));
		model->instances_added();
	}
}