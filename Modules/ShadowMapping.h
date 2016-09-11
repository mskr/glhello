#ifndef SHADOW_MAPPING_H_
#define SHADOW_MAPPING_H_

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "../Module.h"
#include "../Shader.h"
#include "../VertexShader.h"
#include "../FragmentShader.h"
#include "Light.h"
#include "Camera.h"

class ShadowMapping : public Module {

	// Light object contains information about all light sources
	Light* light_;
	// View-projection-matrix for transforming points into light space
	glm::mat4 light_space_matrix_; //TODO Matrix for EACH light source
	// Texture where the depth from the light's point of view is stored
	GLuint depth_texture_; //TODO Depth map for EACH light source
	// Framebuffer holding the depth map texture
	GPUBuffer depth_framebuffer_;
	// Viewport resolution to use when rendering depth
	glm::vec2 depth_map_resolution_;
	// Memorize original viewport resolution to reset after depth map generation
	glm::vec2 original_resolution_;

	// GLuint debug_vao_;
	// GLuint debug_shader_;

public:
	ShadowMapping(Light* light);
	~ShadowMapping();
	
	std::vector<Uniform> uniforms() override;
	int num_passes() override;
	void on_pass(int pass) override;
	GLuint rendertarget(int pass) override;
	void debug_pass(int pass) override;

};

#endif