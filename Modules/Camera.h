#ifndef CAMERA_H_
#define CAMERA_H_

#include "../Module.h"
#include "../World.h"
#include "../User.h"
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
* Multiple cameras currently not possible because
* a) GLFW window creates OpenGL context.
* b) One VAO cannot be used for multiple contexts.
* c) GLEW context is seperat.
*/
class Camera : public Module {

	glm::vec3 position_;
	glm::vec3 target_;
	glm::vec3 up_vector_;

	glm::mat4 view_projection_matrices_[2];

	glm::vec2 viewport_;
	GLFWwindow* window_;

	bool is_on_;

	// These methods receive events of respective interaction type
	void simple(CameraInteraction::Simple* interaction);
	void arcball(CameraInteraction::Arcball* interaction);
	void first_person(CameraInteraction::FirstPerson* interaction);

public:
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up_vector, float field_of_view, glm::vec2 viewport, float near, float far);
	~Camera();

	bool is_on() { return is_on_; }
	glm::vec2 viewport() { return viewport_; }

	void shoot(World* world);

	glm::vec2 transform_world_to_screenspace(glm::vec4 worldspace_coordinates) const;
	glm::vec2 transform_screen_to_texturespace(glm::vec2 screenspace_coordinates) const;

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
	* TODO Implement as module with camera as dependency.
	*/
	struct PostProcessor {
		friend class Camera;
	private:
		bool is_on_;
		Camera* camera_;
		GPUBuffer* framebuffer_;
		GLuint world_image_; //Texture
		GLuint shader_;
		GLuint vao_; //Screen-filling quad
		// Hold samplers in shader_, index == bindingpoint
		std::vector<GLuint> samplers_;
		// Hold uniform locations and update functions
		std::map<GLint, std::function<void(GLint)>> uniform_update_functions_;
		PostProcessor();
		void on(Camera* camera);
		GLuint rendertarget();
		void post_pass();
	public:
		~PostProcessor();
		Camera* camera() { return camera_; }
		void sampler(GLuint texture);
		void uniform(const char* name, std::function<void(GLint)> callback);
	};

	// Getter
	Camera::PostProcessor* post_processor();

private:
	// One camera has one post-processor
	Camera::PostProcessor post_processor_;

};


#endif