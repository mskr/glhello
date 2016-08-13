#ifndef CAMERA_H_
#define CAMERA_H_

#include "../Module.h"
#include "../World.h"
#include "CameraInteraction.h"

/*
* This is a camera.
* You can place a camera in a world.
* The camera can shoot the world.
* The camera renders the shot to a window.
* The image of the world depends on
* a) viewpoint
* b) projection
* which is defined by the camera parameters.
* A camera can be controlled by a user.
*/
class Camera : public Module {

	// GLFWwindow* window_;
	

	glm::vec3 position_;
	glm::vec3 target_;
	glm::vec3 up_vector_;

	glm::mat4 view_projection_matrices_[2];

	// These methods receive events of respective interaction type
	void simple(CameraInteraction::Simple* interaction);
	void arcball(CameraInteraction::Arcball* interaction);
	void first_person(CameraInteraction::FirstPerson* interaction);

public:
	Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up_vector, float field_of_view);
	~Camera();

	void shoot(World* world);

	glm::vec2 transform_world_to_screenspace(glm::vec4 worldspace_coordinates) const;

	void interact(Interaction* interaction) override;
	Interaction* interaction_type() override;
	std::vector<Uniform> uniforms() override;

	//TODO Bind a model to the camera, that is always at the same position (e.g. a first-person player)
	//void model(Model m);
	
	/*
	* The PostProcessor is for post processing effects.
	* It uses a framebuffer with a texture attachment.
	* The texture has the size of the viewport.
	* Post processing has 2 shader passes:
	* 1) Render world normally to texture.
	* 2) Edit texture and render it on fullscreen quad.
	*/
	struct PostProcessor {
		friend class Camera;
	private:
		// Off=true is the default state of the post processor
		bool off_;
		// Render target for the ordinary pass
		GLuint framebuffer_;
		GLuint texture_;
		// GPU program for the post pass
		GLuint post_pass_shader_;
		// Uniforms for the post pass
		std::vector<GLint> uniform_locations_;
		std::function<void(std::vector<GLint>*)> uniform_callback_;
		// Vertices of the fullscreen quad
		GLuint vao_;
		static GLfloat fullscreen_quad_[];
		// Functions for a custom pre pass
		std::function<void()> pre_pass_;
		std::function<void()> post_pre_pass_;
		PostProcessor();
		// Constructor for normal post processing
		PostProcessor(
			GLuint post_pass_shader,
			std::initializer_list<std::string> uniforms,
			std::function<void(std::vector<GLint>*)> uniform_callback
		);
		~PostProcessor();
		// Renders world normally to the texture_
		void ordinary_pass();
		// Runs a shader in order to edit texture_ and renders to screen
		void post_pass();
		// If pre pass defined, executes pre_pass_ and returns true
		bool pre_pass();
		// Called whenever pre pass was called
		void post_pre_pass();
	public:
		void add_pre_pass(
			std::function<void()> pre_pass,
			std::function<void()> post_pre_pass
		);
		// Helper
		void upload_quad_vertices();
		void enable_quad_coords_in_shader(GLuint shader);
		void allocate_offscreen_rendertarget(GLuint* framebuffer, GLuint* texture);
		void locate_post_pass_uniform(const GLchar* uniform_name);
	} post_processor;

	Camera::PostProcessor* post_process(
		GLuint gpu_program, 
		std::initializer_list<std::string> uniforms, 
		std::function<void(std::vector<GLint>*)> uniform_callback
	);

};


#endif