#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <initializer_list>
#include <stdexcept>
#include "stdio.h"
#include "Light.h"
#include "InstanceAttribute.h"

/*
* This is material.
* Material interacts with light.
* The outcome is the color that is rendered.
* Material has 4 properties:
* a) Absorption (Which colors are how much absorped?)
* b) Reflection (Which colors are how much reflected?)
* c) Transmission (How much light passes through the material?)
* d) Shininess (How sleek is the material?)
* You can assign materials to models.
* Materials are independent from geometry.
* Geometry are vertex positions and normals.
* The factory creates geometry.
*/
class Material : public InstanceAttribute {

	struct Array {
		// Number and order of these members must not be changed!
		// The reason is that they are loaded "as are" into a GPU buffer.
		// Default values are a white material (zero absorption, full reflection)
		float absorption_rgb_[3] = {0.0f, 0.0f, 0.0f};
		float reflection_rgb_[3] = {1.0f, 1.0f, 1.0f};
		float transmission_strength_ = 0.0f;
		float shininess_ = 0.0f;
	} properties;

	glm::vec3 get_RGB(float wavelength, float width, float strength);

public:
	// Wavelength in [380,750]: Color, that is most absorped/reflected.
	// Width in [0,1]: Absorped/reflected width of the color spectrum in percent.
	// Strength in [0,1]: Absorped/reflected intensity in percent.
	Material(
		float absorption_wavelength, float absorption_width, float absorption_strength, 
		float reflection_wavelength, float reflection_width, float reflection_strength, 
		float transmission_strength, float shininess
	);
	Material();
	~Material();

	// SETTER
	void absorb(float absorption_wavelength, float absorption_width, float absorption_strength);
	void reflect(float reflection_wavelength, float reflection_width, float reflection_strength);
	void transmit(float transmission_strength);
	void shine(float shininess);
};

#endif