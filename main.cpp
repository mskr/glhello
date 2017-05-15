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
#include <random>

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
#include "Modules/SunShaftsSousa.h"
#include "Modules/OBJLoader.h"
#include "ParticipatingMediaToth.h"


/* Cube test scene */
/*
int main() {
	Camera cam(glm::vec3(0,0,10),
		glm::vec3(0,0,0),
		glm::vec3(0,1,0),
		45,
		glm::vec2(800,450),
		0.1f,
		800.0f);

	Light light({
		{{650.0f, 0.0f, 1.0f}, {0,10,10}}
	});

	GLuint triShader = Shader::link({
		VertexShader("tri.vert"),
		FragmentShader("tri.frag")
	});

	ModelType triModel(0, GL_TRIANGLES, triShader,
		{VertexAttribute("normal")}, {
		Material(),
		Light::Emitter()
	});

	Model cube(0, &triModel, factory.cube());

	World world({&cube}, [](Model* m){}, {});

	world.extend(&cam);
	world.extend(&light);

	User user(&world);
	user.use(&cam);

	glClearColor(0,0,0,1);
	glClearDepth(1);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while(cam.is_on()) {
		cam.shoot(&world);
		glfwPollEvents();
	}

	exit(EXIT_SUCCESS);
}
*/

/* Test scenes used for bachelor thesis */
int main(void) {

	using namespace glm;

	// Camera creates GLFW window and thus OpenGL context
	Camera camera(
		vec3(10,2,44),//vec3(0,3,-12),//glm::vec3(22,6,-21),//vec3(1,-16,2),//vec3(-2,2,9),
		vec3(10,0,0),//glm::vec3(-1,7,6),//vec3(1,4,0), //vec3(-2,8,0 -2,5,0),
		glm::vec3(0,1,0), 
		45,//25, 
		glm::vec2(800,600), 
		0.1f, 
		800.0f);

	printf("* Hello.\n* This is OpenGL %s.\n* The shading language is GLSL %s.\n* Your hardware is %s.\n",
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION),
		glGetString(GL_RENDERER));

	Light light({
		// {{650.0f, 0.0f, 0.3f}, {-15,0,0}}
		// {{650.0f, 0.0f, 0.8f}, {3,10,-60}}
	});

	GLuint shader1 = Shader::link({VertexShader("triangle.vert"), FragmentShader("triangle.frag")});

	GLuint shader2 = Shader::link({VertexShader("point.vert"), FragmentShader("point.frag")});

	ModelType type1(1, GL_TRIANGLES, shader1, {VertexAttribute("normal")}, {
		Material(),
		Light::Emitter()
	});

	ModelType type2(2, GL_POINTS, shader2, {}, {});

	Model cube(1, &type1, factory.cube());
	Model sun(2, &type2, {{{0, 4, 0}}}); // sun as point sprite
	Model tetrahedron(3, &type1, factory.tetrahedron());
	Model octahedron(4, &type1, factory.octahedron());

	cube.attr(1, Material( //TODO how to get rid of the index?
		0.0f, 0.0f, 0.0f,
		730.0f, 1.0f, 0.1f,
		0.0f, 0.0f
	));

	cube.sub({
		//links
		cube.use()->translate(-4,0,2)->units(0.2,4,0.2),
		cube.use()->translate(-4,0,-2)->units(0.2,4,0.2),
		cube.use()->translate(-4,0,0)->units(0.1,4,4),
		//rechts
		cube.use()->translate(4,0,2)->units(0.2,4,0.2),
		cube.use()->translate(4,0,-2)->units(0.2,4,0.2),
		cube.use()->translate(4,0,0)->units(0.1,4,4),
		//hinten
		cube.use()->translate(-2.25,0,-2)->units(3.5,4,0.1),//->attr(1, Material(0,0,0, 800,0,1, 0,0)),
		cube.use()->translate(2.25,0,-2)->units(3.5,4,0.1),
		cube.use()->translate(0,1.5,-2)->units(1,1,0.1),
		//Dach
		cube.use()->translate(-2.25,2,0)->units(3.5,0.1,4),
		cube.use()->translate(2.25,2,0)->units(3.5,0.1,4),
		cube.use()->translate(0,2,-1.25)->units(1,0.1,1.5),
		//Staebe
		cube.use()->translate(0,2,0)->units(1,0.05,0.05),
		cube.use()->translate(0,2,0.5)->units(1,0.05,0.05),
		cube.use()->translate(0,2,1)->units(1,0.05,0.05),
		//Querstaebe
		cube.use()->translate(-0.25,2,0.5)->units(0.05,0.05,2),
		cube.use()->translate(0.25,2,0.5)->units(0.05,0.05,2),
		//Zu Dach
		cube.use()->translate(0,2,1.75)->units(1,0.1,0.5)
	});

	cube.attr(1, Material( //TODO how to get rid of the index?
		0.0f, 0.0f, 0.0f,
		730.0f, 0.2f, 0.5f,
		0.0f, 0.0f
	));

	// ModelInstance* lightcube = cube.use();
	// lightcube->translate(13 -4, 8, -8);
	// lightcube->units(0.8f, 0.8f, 0.8f);
	// lightcube->emit(2, light.l(720.0f, 0.0f, 1.0f)); //TODO light is calculated even if the model is not in the world

	// cube.use()->translateZ(5)->emit(2, light.l(700.0f, 0.0f, 0.9f));

	Model skyscraper(6, &type1, factory.cube());
	skyscraper.translateX(16)->units(1, 6, 1)->translateY(2);
	skyscraper.attr(1, Material(
		0,0,0,
		1000, 0, 1,
		0,0
	));
	skyscraper.use()->translateY(4)->units(0.5, 3, 0.5);
	skyscraper.use()->units(5,2,5)->translate(2,-2,2);
	for(int i = 0; i < 10; i+=1) {
		for(int j = 0; j < 10; j+=1) {
			int min = 0, max = 40;
			int r = rand()%(max-min + 1) + min;
			skyscraper.use()->units(0.5,(float)r/10.0f,0.5)->translateX(6+i)->translateY(-3)->translateZ(j)->attr(1, Material(
				0,0,0,
				0, 1, 0.1,
				0,0
			));
		}
	}

	Model skybox(7, &type1, factory.infacing_cube());
	skybox.attr(1, Material(0.0695, 0.118, 0.244));
	skybox.scaleX(200)->scaleY(100)->scaleZ(100);

	Model ground(5, &type1, factory.cube());
	ground.scaleX(1000)->scaleY(1)->scaleZ(1000)->translateY(-16.4);
	((Material*)ground.attr(1))->reflectRGB(0.1f, 0.1f, 0.1f);

	// OBJLoader obj;
	// Model sibenik(0, &type1, obj.load("C:/Users/mk/Desktop/sibenik.obj"));
	// sibenik.attr(1, Material(0.5f, 0.2f, 0.1f));
	// Model dragon(1, &type1, obj.load("C:/Users/mk/Desktop/dragon.obj"));
	// dragon.attr(1, Material(0.5f, 0.2f, 0.1f));
	// dragon.scale(3)->translateY(5.3f)->translateX(15)->rotateY(-90);

	Model lightcube(9, &type1, factory.cube());
	// lightcube.translate(-15, 20, 60); // SIBENIK SCENE OUTDOOR
	// lightcube.translate(60,20,0); // SIBENIK INDOOR
	lightcube.emit(2, light.l(0.0f, 0.0f, 1.0f));

	// UFO SCENE START
	// Model ufo(0, &type1, obj.load("C:/Users/mk/Desktop/ufo.obj"));
	// ufo.attr(1, Material(0.2f,0.2f,0.2f));
	// ufo.translate(-1,5,0);

	lightcube.translate(0,10,15);
	ground.translateY(-1.0f)->scaleZ(0.05);
	Model tower(10, &type1, factory.cube());
	tower.scaleY(10)->translateZ(-20);
	// Model wall(10, &type1, factory.cube());
	// wall.position(-3,0,0)->units(1,10,40);
	// UFO SCENE END

	//UFO DRAGON SCENE START
	// lightcube.translateY(20);
	// ufo.translateY(5);
	// dragon.scale(4)->rotateY(180);

	World world({&cube}, [&](Model* m){
		// if(m->id()==7) m->scaleZ(1.001);
	}, {
		Uniform("modelID", [](Uniform* u, Model* m) {
			u->update(m->id());
		})
	});

	world.extend(&camera);
	world.extend(&light);

	// VolumetricLightScatteringMitchell modLightScattering(camera.post_processor(), lightcube.position_world_space());
	// world.extend(&modLightScattering);

	// SunShaftsSousa modSunShafts(camera.post_processor(), lightcube.position_world_space());
	// world.extend(&modSunShafts);

	// ShadowMapping modShadowMapping(lightcube.position_world_space(), glm::vec3(0,0,4));
	// world.extend(&modShadowMapping);

	// ParticipatingMediaToth toth;
	// world.extend(&toth);

	User user(&world);
	((CameraInteraction*) user.use(&camera))->simple();
	// user.use(&modLightScattering);
	// user.use(&modSunShafts);
	// user.use(&toth);

	//+// Point primitives with shader-specified size
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//+// Background color and depth values
	glClearColor(0.0695*4, 0.118*4, 0.244*4, 1.0f);
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

	// double t[100];
	// unsigned int i = 0;
	// rendering loop
	while(camera.is_on()) {
		// double t1 = glfwGetTime();
		camera.shoot(&world);
		// double t2 = glfwGetTime();
		// t[i%100] = t2-t1;
		// i++;
		// if(i%100 == 0) {
		// 	double sum = 0.0;
		// 	for(int j = 0; j < 100; j++) sum += t[j];
		// 	printf("%f\n", sum/100.0);
		// }
		glfwPollEvents();
	}

	exit(EXIT_SUCCESS);
}