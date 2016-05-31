#include "Material.h"

Material::Material() {
	//TODO init standard material here and remove Array properties?
}

Material::Material(
  float absorption_wavelength, float absorption_width, float absorption_strength, 
  float reflection_wavelength, float reflection_width, float reflection_strength, 
  float transmission_strength, float shininess) {

	float h_absorption = Light::wavelength_to_hue(glm::clamp(absorption_wavelength, 380.0f, 750.f));
	float s_absorption = 1.0f - absorption_width;
	float v_absorption = 1.0f - absorption_strength;

	float h_reflection = Light::wavelength_to_hue(glm::clamp(reflection_wavelength, 380.0f, 750.0f));
	float s_reflection = 1.0f - reflection_width;
	float v_reflection = reflection_strength;

	glm::vec3 absorption_rgb = Light::hsv_to_rgb(h_absorption, s_absorption, v_absorption);
	glm::vec3 reflection_rgb = Light::hsv_to_rgb(h_reflection, s_reflection, v_reflection);
	properties.absorption_rgb_[0] = absorption_rgb.x;
	properties.absorption_rgb_[1] = absorption_rgb.y;
	properties.absorption_rgb_[2] = absorption_rgb.z;
	properties.reflection_rgb_[0] = reflection_rgb.x;
	properties.reflection_rgb_[1] = reflection_rgb.y;
	properties.reflection_rgb_[2] = reflection_rgb.z;
	properties.transmission_strength_ = transmission_strength;
	properties.shininess_ = shininess;

	InstanceAttribute::bytes_ = sizeof(Material::Array);
	InstanceAttribute::pointer_ = (GLvoid*) &properties;
}

//TODO
//Material(Texture texture); //Texture objects contain color, specular and diffuse parameters

Material::~Material() {
	
}

InstanceAttribute Material::instance_attrib([](GLuint gpu_program, GLsizei offset, GLsizei stride) {
	GLuint loc_absorption = glGetAttribLocation(gpu_program, "MaterialAbsorption");
	GLuint loc_reflection = glGetAttribLocation(gpu_program, "MaterialReflection");
	GLuint loc_transmission = glGetAttribLocation(gpu_program, "MaterialTransmission");
	GLuint loc_shininess = glGetAttribLocation(gpu_program, "MaterialShininess");
	glVertexAttribPointer(loc_absorption, 3, GL_FLOAT, GL_FALSE, stride,
		(GLvoid*) (offset + 0)
	);
	glVertexAttribPointer(loc_reflection, 3, GL_FLOAT, GL_FALSE, stride,
		(GLvoid*) (offset + 3 * sizeof(GLfloat))
	);
	glVertexAttribPointer(loc_transmission, 1, GL_FLOAT, GL_FALSE, stride,
		(GLvoid*) (offset + 6 * sizeof(GLfloat))
	);
	glVertexAttribPointer(loc_shininess, 1, GL_FLOAT, GL_FALSE, stride,
		(GLvoid*) (offset + 7 * sizeof(GLfloat))
	);
	glVertexAttribDivisor(loc_absorption, 1);
	glVertexAttribDivisor(loc_reflection, 1);
	glVertexAttribDivisor(loc_transmission, 1);
	glVertexAttribDivisor(loc_shininess, 1);
	glEnableVertexAttribArray(loc_absorption);
	glEnableVertexAttribArray(loc_reflection);
	glEnableVertexAttribArray(loc_reflection);
	glEnableVertexAttribArray(loc_shininess);
});