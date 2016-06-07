//TODOs (big ones):
/*
/ 1. Collection of ready to draw 3D-primitives
/ 2. Transparency with Depth Peeling
/ 3. Divide program in core and modules.
/ 4. Physical Simulation with Transform Feedback (OpenGL Superbible Example)
/ 5. Shadow Volumes
/ 6. Unified Particle Physics for Real-Time Applications with compute shader
/ 7. Indirect Illumination with Voxel Cone Tracing
/ 8. Heightmap generator for microstructure materials (e.g. textiles), render with normal mapping
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

#include "config.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "Camera.h"
#include "PostProcessor.h"
#include "Light.h"
#include "User.h"
#include "Factory.h"

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

GLFWwindow* setupContext(const char* title) {
	// create the opengl context containing an opengl state machine
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if(!glfwInit()) throw std::runtime_error("Program exits because glfwInit failed.");
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(config::viewport_width, config::viewport_height, title, NULL, NULL);
	if(!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	// enabling platform-independent opengl 3 functions
	glewExperimental = GL_TRUE;
	glewInit();
	// show infos in console
	printf("* Hello.\n* This is OpenGL %s.\n* The shading language is GLSL %s.\n* Your hardware is %s.\n",
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION),
		glGetString(GL_RENDERER));
	return window;
}


int main(void) {
	
	GLFWwindow* window = setupContext("Hello World");

	GLuint shader1 = Shader::link({
		VertexShader("triangleShader.vert"),
		FragmentShader("triangleShader.frag")
	});

	ModelType type0(0, GL_TRIANGLES, shader1, {
		VertexAttribute("color"),
		VertexAttribute("normal")
	});

	Model m0(0, &type0, {});
	m0.vertices(factory.checkerboard(64));
	m0.units(64,0,64);
	m0.translate(-32,0,-32);

	ModelType type1(1, GL_TRIANGLES, shader1, {
		VertexAttribute("normal")
	});
	type1.instance_attribs({ Material::instance_attrib });

	Model m1(1, &type1, {Material(
		380.0f, 0.5f, 0.0f,
		750.0f, 1.0f, 1.0f,
		0.0f, 0.0f
	)});
	m1.vertices(factory.cube());
	m1.units(1,1,1);
	m1.translate(0.5f, 0.0f, 0.5f);
	m1.use()->translateX(4)->units(1,30,1)->attr(1, Material(
		750.0f, 0.5f, 0.0f,
		380.0f, 1.0f, 1.0f,
		0.0f, 0.0f
	));

	World world({&m0, &m1}, [](Model* m){}, {Uniform("modelID", [](Uniform* u, Model* m) {
		u->update(m->id());
	})});

	Camera camera(glm::vec3(0,0,10), glm::vec3(0,0,0), glm::vec3(0,1,0), 
		90, config::viewport_width/config::viewport_height, 0.1f, 100.0f);
	camera.use(PostProcessor(Shader::link({
		VertexShader("PostProcess.vert"),
		FragmentShader("PostProcess.frag")
	})));
	world.add(&camera);

	Light light({
		{{650.0f, 0.0f, 1.0f}, {10, 50, 10}},
		{{650.0f, 0.0f, 1.0f}, {10, 0, 10}}
	});
	world.add(&light);

	User user(&world);
	user.use(window);
	((CameraInteraction*) user.use(&camera))->simple();

	//+// Point primitives with shader-specified size
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//+// Default color and depth values
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
	while(!glfwWindowShouldClose(window)) {
		camera.shoot(&world);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// terminating properly when rendering loop exited
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}