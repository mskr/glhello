#ifndef MODULE_H_
#define MODULE_H_

#include <vector>

#include "Interaction.h"
#include "Uniform.h"
#include "Shader.h"
#include "VertexShader.h"
#include "FragmentShader.h"

/*
* This is the base class for modules.
* Any future feature of this graphics framework should extend Module.
* Modules can provide uniforms to world's shaders.
* Modules can provide an extra shader to the world.
* The extra shader runs on the world's models.
* The extra shader is invoked before the ordinary render pass.
* You must define the number of passes your extra shader needs.
* Modules are automatically enabled for user interactions.
*/
class Module {

protected:

	/*
	* A modeltype-independet shader.
	* Modules can use it to perform one or more pre-render-passes on the world's geometry.
	* It renders to the framebuffer given by Module::rendertarget().
	* It runs once on each modeltype's attribute buffers.
	* You can decide to use a subset of their attributes.
	* You should not use attributes that are not present in one or more buffers.
	* Default are
	* a) vertex attribute "position" and
	* b) instance attribute "model".
	* Extend with
	* a) add_attribute(type, name)
	* b) add_uniform(type, name)
	*    - add a uniform that you have to update
	* c) add_varying(interpolation_mode, type, vert_name, frag_name)
	* d) add_sampler2D(binding, name)
	* e) add_matrix(name)
	*    - to add a uniform mat4 and multiply it with the (already transformed) position
	* f) set_frag_color(statement_evaluating_to_vec4)
	* Call compile_and_link() when you are ready.
	* If you don't call this, the world skips the pre-pass.
	*/
	struct shader {
		GLuint gpu_program = 0;
		std::vector<Uniform> simple_uniforms;
		std::vector<Uniform> uniform_buffers;
		std::string uniforms = "";
		std::string attributes = "attribute vec3 position; attribute mat4 model;";
		std::string samplers = "";
		std::string varyings = "";
		std::string varying_assignments = "";
		std::string matrices = "";
		std::string frag_output = "";
		void add_uniform(std::string type, std::string name) {
			uniforms += "uniform " + type + " " + name + ";";
		}
		void add_uniform_block(std::string name, std::vector<std::vector<std::string>> contents) {
			uniforms += "layout(std140) uniform " + name + "{";
			for(std::vector<std::string> type_and_name : contents)
				uniforms += type_and_name[0] + " " + type_and_name[1] + ";";
			uniforms += "};";
		}
		void add_storage_buffer(std::string name, std::vector<std::vector<std::string>> contents) {
			uniforms += "layout(std430) buffer " + name + "{";
			for(std::vector<std::string> type_and_name : contents)
				uniforms += type_and_name[0] + " " + type_and_name[1] + ";";
			uniforms += "};";
		}
		void add_attribute(std::string type, std::string name) {
			attributes += "attribute " + type + " " + name + ";";
		}
		void add_varying(std::string interpolation_mode, std::string type, std::string vert_name, std::string frag_name) {
			varyings += interpolation_mode + " varying " + type + " " + frag_name + ";";
			varying_assignments += frag_name + " = " + vert_name + ";";
		}
		void add_sampler2D(std::string binding, std::string name) {
			samplers += "layout(binding=" + binding + ") uniform sampler2D " + name + ";";
		}
		void add_matrix(std::string name) {
			matrices += name + "*";
		}
		void set_frag_color(std::string statement) {
			frag_output = "gl_FragColor = " + statement + ";";
		}
		void compile_and_link() {
			gpu_program = Shader::link({VertexShader({
				"#version 430",
				attributes,
				uniforms,
				varyings,
				"void main() {",
				varying_assignments,
				"	gl_Position = " + matrices + " model * vec4(position,1);",
				"}"
			}), FragmentShader({
				"#version 430",
				varyings,
				uniforms,
				samplers,
				"void main() {",
				frag_output,
				"}"
			})});
		}
	} shader_;

public:
	Module() {}
	virtual ~Module() {}

	// Receives and processes user interaction events
	virtual void interact(Interaction* i) {}

	// Returns the special interaction type of this module
	virtual Interaction* interaction_type() { return new Interaction(); }

	// Constructs and returns uniforms that this module wants to provide to other shaders
	// Before this call the module must be completely *inactive*
	virtual std::vector<Uniform> uniforms() { return std::vector<Uniform>{}; }

	// Returns the pre-pass shader
	GLuint shader() { return shader_.gpu_program; }

	// Returns the number of the shader's passes
	virtual int num_passes() { return 0; }

	// Called before each pass
	virtual void on_pass(int pass) {}

	// Return a framebuffer object or 0 for default framebuffer
	virtual GLuint rendertarget(int pass) { return 0; }

	// Called after each pass
	virtual void debug_pass(int pass) {}

	// Called by world in order ot make all uniforms accessable to all modules
	void add(Uniform u) {
		if(u.is_ubo() && u.has_bytes()) {
			u.gpu_program(shader_.gpu_program);
			shader_.uniform_buffers.push_back(u);
		} else if(u.is_ubo()) {
			// Uniform is a shader storage buffer
			u.gpu_program(shader_.gpu_program);
			shader_.uniform_buffers.push_back(u);
		} else {
			u.gpu_program(shader_.gpu_program);
			shader_.simple_uniforms.push_back(u);
		}
	}

	// Called inside World::draw_modules()
	void update() {
		for(Uniform &u : shader_.uniform_buffers)
			u.callback();
	}

	// Called inside World::draw_modules() for each model
	void update(Model* m) {
		for(Uniform &u : shader_.simple_uniforms)
			u.callback(m);
	}

};

#endif