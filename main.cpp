//TODOs (big ones):
/*
/ 0. Rework user input as described in User.cpp
/ 1. Collection of ready to draw 3D-primitives
/ 2. Transparency with Depth Peeling
/ 3. Divide program in core and modules (e.g. camera is a module). Use world.add(module).
/ 4. Physical Simulation with Transform Feedback (OpenGL Superbible Example)
/ 5. Shadow Volumes
/ 6. Unified Particle Physics for Real-Time Applications with compute shader
/ 7. Inderect Illumination with Voxel Cone Tracing
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

	ModelType poly(0, GL_TRIANGLES, shader1, {
		VertexAttribute("color"),
		VertexAttribute("normal")
	});

	Model m0(0, &poly);
	m0.vertices(factory.cube());

	Model bigcube(3, &poly);
	bigcube.vertices(factory.infacing_cube());
	bigcube.unitsY(10);

	Model m1(1, &poly);
	m1.vertices(factory.checkerboard(64));
	m1.units(64,0,64);
	m1.translate(-32,0,-32);

	GLuint shader2 = Shader::link({
		VertexShader("particleShader.vert"),
		FragmentShader("particleShader.frag")
	});
	ModelType particle(1, GL_POINTS, shader2, {
		VertexAttribute("color")
	});
	Model m2(2, &particle, {{{10,0,0},{1,1,1}}});
	Model m3(3, &particle, {{{-10,0,10},{1,1,1}}});

	World world({&m0, &m1, &m2, &m3, &bigcube}, [](Model* m){
		// if(m->id()==0) m->instance(1)->rotateX(0.0001f);
	}, {/* no uniforms */});

	// CAMERA PARAMS: world, position, target, up_vector, fov in degrees, screenratio, near, far
	Camera camera(glm::vec3(0,0,2), glm::vec3(0,0,0), glm::vec3(0,1,0), 
		90, config::viewport_width/config::viewport_height, 0.1f, 100.0f);
	world.add(&camera);

	Light light({
		{{350.0f, 0.0f, 1.0f}, {10, 0, 0}}, // amplitude not working?
		{{350.0f, 0.0f, 1.0f}, {-10, 0, 10}} // amplitude not working?
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
	//glDepthMask(GL_FALSE); // for disabling depth buffer writing

	// rendering loop
	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera.shoot(&world);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// terminating properly when rendering loop exited
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

	/*
	// bind new framebuffer used for depth peeling post processing
	GLuint framebuffer2;
	glGenFramebuffers(1, &framebuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer2);
	// create texture that holds the rendered pixels
	GLuint colorTex2; 						// COLOR TEX 2
	glGenTextures(1, &colorTex2);
	glBindTexture(GL_TEXTURE_2D, colorTex2);
	// init texture with screen-aligned size
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, config::viewport_width, config::viewport_height, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// create texture that holds the rendered pixels
	GLuint zTex2; 							// Z TEX 2
	glGenTextures(1, &zTex2);
	glBindTexture(GL_TEXTURE_2D, zTex2);
	// init texture with screen-aligned size
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, config::viewport_width, config::viewport_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex2, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, zTex2, 0);
	*/