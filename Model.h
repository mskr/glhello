#ifndef MODEL_H_
#define MODEL_H_

#include <vector>

#include "ModelInstance.h"
#include "ModelType.h"
#include "GPUBuffer.h"
#include "Material.h"
#include "stdio.h"
#include "config.h"
#include <limits>

/*
* This is a 3D model.
* It stores vertex data.
* It must have a model-type, that defines
* a) the drawing primitive
* b) the shader
* c) the vertex attributes
* for this model.
* The vertex attribute "position" is available per default.
* The position must have 3+ components.
* The model can also have instances (and is an instance itself).
* Instances use the same vertex data.
* Each instance uses a different matrix.
* All matrices are stored subsequently.
*/
class Model : public ModelInstance {
	ModelType* modeltype_;
	GLint num_vertices_;
	// Vertices with their attributes in contiguous storage locations (as they will be in gpu memory)
	std::vector<GLfloat> vertices_;

	// Instances of this model, including this object itself at index 0 (array index equals instance_id)
	std::vector<ModelInstance*> instances_;

	// Model matrices (array index equals instance_id)
	std::vector<glm::mat4> matrices_;

	// Counts how often this model has been use()d since last draw
	int num_new_instances_;

public:
	Model(int id, ModelType* modeltype);
	Model(int id, ModelType* modeltype, std::initializer_list<std::initializer_list<std::initializer_list<GLfloat>>> v);
	Model(int id, ModelType* modeltype, std::vector<std::vector<std::vector<GLfloat>>> v);
	~Model();

	// Returns instance of this model, which can be seperately transformed (hardware instancing)
	ModelInstance* use();
	//TODO
	//ModelInstance* use(glm::vec3 pos); // directly translate instance after use

	void draw(GLint offset, GLuint instance_attribs_offset);

	GLsizeiptr bytes() { return vertices_.size() * sizeof(GLfloat); }
	const GLvoid* pointer() { return vertices_.data(); }

	GLsizeiptr bytes_matrices() { return matrices_.size() * sizeof(matrices_[0]); }
	const GLvoid* pointer_matrices() { return matrices_.data(); }

	GLsizeiptr bytes_instance_attribs();
	void update_instance_attribs(GPUBuffer* b, GLint offset);
	const GLvoid* pointer_instance_attr(unsigned int instance_index, unsigned int attr_index);

	// GETTER
	ModelType* modeltype() { return modeltype_; }
	GLint num_vertices() { return num_vertices_; }
	GLsizei num_instances() { return instances_.size(); }
	GLsizei num_matrices() { return matrices_.size(); }
	ModelInstance* instance(int instance_id) { return instances_[instance_id]; }
	bool num_new_instances() { return num_new_instances_; }
	glm::mat4* matrix_at(int index) { return &matrices_[index]; }

	// SETTER
	void vertices(std::vector<std::vector<std::vector<GLfloat>>> vertices);
	void instances_added();

	
	//TODO 

	// Draw this model independently from a world (own vao and vbo required)
	// void draw_free();

	friend class ModelInstance; // allows model instances to see private members
};


#endif