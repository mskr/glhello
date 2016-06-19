#include "Material.h"

Material::Material(
  float absorption_wavelength, float absorption_width, float absorption_strength, 
  float reflection_wavelength, float reflection_width, float reflection_strength, 
  float transmission_strength, float shininess) {
	glm::vec3 absorption_rgb = get_RGB(absorption_wavelength, absorption_width, absorption_strength);
	glm::vec3 reflection_rgb = get_RGB(reflection_wavelength, reflection_width, reflection_strength);
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
	InstanceAttribute::index_func_ = [](unsigned int i) {
		// i is the index of this instance attrib in the model instance's attribs list
		Material::instance_attrib.set_index(i);
	};
}

//TODO
//Material(Texture texture); //Texture objects contain color, specular and diffuse parameters

Material::~Material() {
}

glm::vec3 Material::get_RGB(float wavelength, float width, float strength) {
	return Light::hsv_to_rgb(
		Light::wavelength_to_hue(glm::clamp(wavelength, 380.0f, 750.0f)),
		glm::clamp(1.0f - width, 0.0f, 1.0f),
		glm::clamp(strength, 0.0f, 1.0f)
	);
}

// SETTER
void Material::absorb(float absorption_wavelength, float absorption_width, float absorption_strength) {
	glm::vec3 absorption_rgb = get_RGB(absorption_wavelength, absorption_width, absorption_strength);
	properties.absorption_rgb_[0] = absorption_rgb.x;
	properties.absorption_rgb_[1] = absorption_rgb.y;
	properties.absorption_rgb_[2] = absorption_rgb.z;

	InstanceAttribute::has_changed_ = true;
}

void Material::reflect(float reflection_wavelength, float reflection_width, float reflection_strength) {
	glm::vec3 reflection_rgb = get_RGB(reflection_wavelength, reflection_width, reflection_strength);
	properties.reflection_rgb_[0] = reflection_rgb.x;
	properties.reflection_rgb_[1] = reflection_rgb.y;
	properties.reflection_rgb_[2] = reflection_rgb.z;

	InstanceAttribute::has_changed_ = true;
}

void Material::transmit(float transmission_strength) {
	properties.transmission_strength_ = transmission_strength;
	InstanceAttribute::has_changed_ = true;
}

void Material::shine(float shininess) {
	properties.shininess_ = shininess;
	InstanceAttribute::has_changed_ = true;
}

// Function for setting and enabling vertex attrib pointers.
InstanceAttribute Material::instance_attrib([](GLuint gpu_program, GLsizei offset, GLsizei stride) {
	GLint loc_absorption = glGetAttribLocation(gpu_program, "MaterialAbsorption");
	if(loc_absorption == -1) printf("WARNING: Attribute \"MaterialAbsorption\" not found in shader.\n");
	else {
		glVertexAttribPointer(loc_absorption, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (offset + 0));
		glVertexAttribDivisor(loc_absorption, 1);
		glEnableVertexAttribArray(loc_absorption);
	}
	GLint loc_reflection = glGetAttribLocation(gpu_program, "MaterialReflection");
	if(loc_reflection == -1) printf("WARNING: Attribute \"MaterialReflection\" not found in shader.\n");
	else {
		glVertexAttribPointer(loc_reflection, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (offset + 3 * sizeof(GLfloat)));
		glVertexAttribDivisor(loc_reflection, 1);
		glEnableVertexAttribArray(loc_reflection);
	}
	GLint loc_transmission = glGetAttribLocation(gpu_program, "MaterialTransmission");
	if(loc_transmission == -1) printf("WARNING: Attribute \"MaterialTransmission\" not found in shader.\n");
	else {
		glVertexAttribPointer(loc_transmission, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (offset + 6 * sizeof(GLfloat)));
		glVertexAttribDivisor(loc_transmission, 1);
		glEnableVertexAttribArray(loc_transmission);
	}
	GLint loc_shininess = glGetAttribLocation(gpu_program, "MaterialShininess");
	if(loc_shininess == -1) printf("WARNING: Attribute \"MaterialShininess\" not found in shader.\n");
	else {
		glVertexAttribPointer(loc_shininess, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*) (offset + 7 * sizeof(GLfloat)));
		glVertexAttribDivisor(loc_shininess, 1);
		glEnableVertexAttribArray(loc_shininess);
	}
});