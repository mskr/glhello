//TODOs (big ones):
/*
/ 1. Collection of ready to draw 3D-primitives
/ 2. Transparency with Depth Peeling
/ 4. Physical Simulation with Transform Feedback (OpenGL Superbible Example)
/ 5. Shadow Volumes en.wikipedia.org/wiki/Shadow_volume
/ 6. Unified Particle Physics for Real-Time Applications with compute shader
/ 7. Indirect Illumination with Voxel Cone Tracing
/ 8. Heightmap generator for microstructure materials (e.g. textiles), render with normal mapping
/ 9. Include V8 and manipulate the scene at runtime using JavaScript.
*/

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtc/matrix_inverse.hpp>
#include <gtc/matrix_access.hpp>
#include <gtc/quaternion.hpp>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>

#include "config.h"
#include "ModelType.h"
#include "Shader.h"
#include "FragmentShader.h"
#include "VertexShader.h"
#include "Module.h"
#include "Model.h"
#include "Factory.h"
#include "World.h"
#include "User.h"

#include "Modules/Camera.h"
#include "Modules/VolumetricLightScatteringMitchell.h"
#include "Modules/ShadowMapping.h"

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

int main(void) {
	glfwSetErrorCallback(error_callback);
	if(!glfwInit()) throw std::runtime_error("Program exits because GLFW init failed.");
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Camera creates GLFW window and thus OpenGL context
	Camera camera(glm::vec3(2,3,15), glm::vec3(0,0,0), glm::vec3(0,1,0), 45, glm::vec2(800,600), 0.1f, 800.0f);

	glewExperimental = GL_TRUE;
	glewInit();

	printf("* Hello.\n* This is OpenGL %s.\n* The shading language is GLSL %s.\n* Your hardware is %s.\n",
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION),
		glGetString(GL_RENDERER));

	Light light({
		 // {{650.0f, 0.0f, 1.0f}, {0,2,20}}
	});

	GLuint shader1 = Shader::link({VertexShader("triangle.vert"), FragmentShader("triangle.frag")});

	GLuint shader2 = Shader::link({VertexShader("point.vert"), FragmentShader("point.frag")});

	ModelType type1(1, GL_TRIANGLES, shader1, {VertexAttribute("normal")}, {
		Material(),
		Light::Emitter()
	});

	ModelType type2(2, GL_POINTS, shader2, {}, {});

	Model cube(1, &type1, factory.cube());
	Model sun(2, &type2, {{{12, 20, -30}}}); // sun as point sprite
	Model t(2, &type1, factory.cube());

	cube.attr(1, Material( //TODO how to get rid of the index?
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f
	));
	cube.units(1, 5, 1);
	ModelInstance* ground = cube.use()->units(800, 2, 800)->translateY(-2);
	ground->attr(1, Material(
		0.0f, 0.0f, 0.0f,
		570.0f, 0.5f, 0.8f, // gray
		0.0f, 0.0f
	));
	// for(int x = 0; x < 80; x+=4) {
	// 	for(int z = 0; z < 80; z+=4) {
	// 		if(x==0 && z==0) continue;
	// 		ModelInstance* skyscraper = cube.use()->translateX(x)->translateZ(z);
	// 		skyscraper->attr(1, Material(
	// 			0.0f, 0.0f, 0.0f,
	// 			500.0f, 0.2f, 0.8f, // blue
	// 			0.0f, 0.0f
	// 		));
	// 	}
	// }
	ModelInstance* lightcube = cube.use()->units(0.1f, 0.1f, 0.1f)->translate(2,2,-2);
	lightcube->emit(2, light.l(0.0f, 0.0f, 1.0f)); //TODO light is calculated even if the model is not in the world

	// t.attr(1, Material(
	// 	0.0f, 0.0f, 0.0f,
	// 	500.0f, 1.0f, 1.0f, // white
	// 	0.0f, 0.0f
	// ));
	// t.position(1,0,0);
	// t.units(1,1,1);
	// t.position(0,0,0);

	World world({&cube}, [](Model* m){
		m->instance(0)->rotateY(0.001f);
	}, {});

	world.extend(&camera);

	VolumetricLightScatteringMitchell modLightScattering(camera.post_processor(), lightcube->position_world_space());

	world.extend(&modLightScattering);

	world.extend(&light);

	ShadowMapping modShadowMapping(&light);

	world.extend(&modShadowMapping);

	User user(&world);
	((CameraInteraction*) user.use(&camera))->simple();
	// user.use(&modLightScattering);

	//+// Point primitives with shader-specified size
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//+// Background color and depth values
	glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
	glClearDepth(1.0);

	//+// Backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

	//+// Alpha channel and blending
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//+// Depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// rendering loop
	while(camera.is_on()) {
		camera.shoot(&world);
		glfwPollEvents();
	}

	// terminating properly when rendering loop exited
	glfwTerminate();
	exit(EXIT_SUCCESS);
}