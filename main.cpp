//TODOs (big ones):
/*
/ 1. Collection of ready to draw 3D-primitives
/ 2. Transparency with Depth Peeling
/ 4. Physical Simulation with Transform Feedback (OpenGL Superbible Example)
/ 5. Shadow Volumes
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
#include "Shader.h"
#include "FragmentShader.h"
#include "VertexShader.h"
#include "ModelType.h"
#include "Material.h"
#include "Model.h"
#include "Factory.h"
#include "World.h"
#include "User.h"

#include "Modules/Camera.h"
#include "Modules/VolumetricLightScatteringMitchell.h"

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
	
	GLFWwindow* window = setupContext("Hello World"); //TODO unify window and camera (Don't forget viewport configs etc.)

	Light light({
		 // {{650.0f, 0.0f, 1.0f}, {0,2,20}}
	});

	GLuint shader1 = Shader::link({
		VertexShader("triangle.vert"),
		FragmentShader("triangle.frag")
	});

	GLuint shader2 = Shader::link({
		VertexShader("point.vert"),
		FragmentShader("point.frag")
	});

	ModelType type1(1, GL_TRIANGLES, shader1, {
		VertexAttribute("normal")
	});
	type1.instance_attribs({ Material::instance_attrib });

	ModelType type2(2, GL_POINTS, shader2, {});

	Model cube(1, &type1, {Material(
		0.0f, 0.0f, 0.0f,
		500.0f, 0.2f, 0.8f, // blue
		0.0f, 0.0f
	)});
	cube.vertices(factory.cube());
	cube.units(1, 10, 1);
	ModelInstance* ground = cube.use()->units(800, 2, 800)->translateY(-1);
	ground->attr(Material::instance_attrib.index(), Material(
		0.0f, 0.0f, 0.0f,
		570.0f, 0.5f, 0.8f, // gray
		0.0f, 0.0f
	));
	for(int x = 0; x < 80; x+=4) {
		for(int z = 0; z < 80; z+=4) {
			if(x==0 && z==0) continue;
			cube.use()->translateX(x)->translateZ(z);
		}
	}
	ModelInstance* lightcube = cube.use()->units(1,1,1)->scale(20)->translateZ(-70)->translateY(50);
	lightcube->emit(light.l(780.0f, 0.5f, 0.9f));


	// Model sun(2, &type2, {{{12, 20, -30}}}, {}); // sun as point sprite
	// sun.emit(light.l(780.0f, 0.0f, 1.0f));

	World world({&cube}, [](Model* m){}, {Uniform("modelID", [lightcube](Uniform* u, Model* m) {
		u->update(m->id());
	})});

	Camera camera(glm::vec3(0,0,20), glm::vec3(0,0,0), glm::vec3(0,1,0), 90);

	VolumetricLightScatteringMitchell lightscattering(&camera, lightcube->position_world_space());

	world.extend(&lightscattering);

	world.extend(&camera);

	world.extend(&light);

	User user(&world);
	user.use(window);
	((CameraInteraction*) user.use(&camera))->simple();

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