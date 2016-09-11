#include "Material.h"

Material::Material(
  float absorption_wavelength, float absorption_width, float absorption_strength, 
  float reflection_wavelength, float reflection_width, float reflection_strength, 
  float transmission_strength, float shininess) : InstanceAttribute(sizeof(Material::Array), (GLvoid*) &properties) {

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
}

Material::Material() : InstanceAttribute(sizeof(Material::Array), (GLvoid*) &properties,
  "MaterialAbsorption:fff,MaterialReflection:fff,MaterialTransmission:f,MaterialShininess:f.") {
	// Not touching properties (leaving everything default)
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