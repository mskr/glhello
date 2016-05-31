#ifndef LIGHT_H_
#define LIGHT_H_

#include "Module.h"
#include <glm.hpp>

/*
* This is light.
* It has 3 properties:
* a) The dominant wavelength (the one color that is most intense)
* b) Monochromaticity (how dominant is the given wavelength in percent?)
* b) Amplitude (Overall intensity of the light)
* Light is normally made up of many wavelengths.
* If more than one wavelength is dominant, you need more than one light.
* The 3 properties define the SDF (Spectral Density Function).
* A color in HSL is given by these properties.
* The HSL color is converted to RGB.
* Every light additionally has a source.
* You specify the source with its x,y,z-position.
* All in all you create Light like this:
* Light({
*   {wavelength, monochromaticity, amplitude}, {x, y, z}, // one source
*   ... // as many sources as you like
* });
*/
class Light : public Module {

	// Light source positions and RGB colors in alternating order
	std::vector<glm::vec3> buffer_;
	unsigned int num_new_lights_;

public:

	// Light({{{wavelength, monochromaticity, amplitude}, {x,y,z}}, ...})
	// Wavelength in [380,750], monochromaticity in [0,1], amplitude in [0,1], otherwise clamped.
	Light(std::initializer_list<std::initializer_list<std::initializer_list<GLfloat>>> in);
	~Light();

	void add(std::initializer_list<std::initializer_list<GLfloat>> light);

	// Module overrides
	void interact(Interaction* i);
	Interaction* interaction_type();
	std::vector<Uniform> uniforms();

	// Wavelength w in [380,750], otherwise undefined output.
	// Output is hue in [0,360].
	static float wavelength_to_hue(float w);

	// Hue h in [0,360], saturation s in [0,1] and value in [0,1], otherwise undefined output.
	// In HSV, white is only achieved with saturation=0.0 and value=1.0.
	// Output is r,g and b in [0,1].
	static glm::vec3 hsv_to_rgb(float h, float s, float v);
};

#endif