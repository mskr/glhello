#include "Light.h"
#include "math.h"
#include "stdio.h"

Light::Light(std::initializer_list<std::initializer_list<std::initializer_list<GLfloat>>> in) {
	num_new_lights_ = 0;
	for(std::initializer_list<std::initializer_list<GLfloat>> tupel : in) add(tupel);
}

Light::~Light() {
}

void Light::add(std::initializer_list<std::initializer_list<GLfloat>> tupel) {
	if(tupel.size() != 2 || (*(tupel.begin()+0)).size() != 3 || (*(tupel.begin()+1)).size() != 3) {
		printf("Light source not added because not of form {{wavelength, monochromaticity, amplitude}, {x,y,z}}.");
		return;
	}
	GLfloat w = *((*(tupel.begin()+0)).begin()+0); // this is like array access at [i][j]
	GLfloat m = *((*(tupel.begin()+0)).begin()+1);
	GLfloat a = *((*(tupel.begin()+0)).begin()+2);
	GLfloat wavelength = glm::clamp(w, 380.0f, 750.0f); // visible light only
	GLfloat monochromaticity = glm::clamp(m, 0.0f, 1.0f);
	GLfloat amplitude = glm::clamp(a, 0.0f, 1.0f);
	GLfloat source_x = *((*(tupel.begin()+1)).begin()+0);
	GLfloat source_y = *((*(tupel.begin()+1)).begin()+1);
	GLfloat source_z = *((*(tupel.begin()+1)).begin()+2);
	buffer_.push_back(glm::vec3(source_x, source_y, source_z));
	glm::vec3 rgb = Light::hsv_to_rgb(Light::wavelength_to_hue(wavelength), monochromaticity, amplitude);
	buffer_.push_back(rgb);
	num_new_lights_++;
}




std::vector<Uniform> Light::uniforms() {
	return std::vector<Uniform> {
		Uniform("light", buffer_.data(), [this](Uniform* u) {
			auto it = buffer_.end() - 2*num_new_lights_;
			glm::vec3* ptr = buffer_.data() + (it - buffer_.begin());
			u->update_and_grow(ptr, 2*num_new_lights_*sizeof(glm::vec3));
			num_new_lights_ = 0;
		}),
		Uniform("num_lights", [this](Uniform* u, Model* m) {
			u->update((int)(buffer_.size()/2));
		})
	};
}

void Light::interact(Interaction* i) {

}

Interaction* Light::interaction_type() {
	return new Interaction();
}




float Light::wavelength_to_hue(float w) {
	// convert wavelength in nanometer to hue in degrees
	// violet is 380 nm resp. 300 deg
	// red is 750 nm resp. 0 deg
	// magenta (300-360 deg) is not existent in wavelength spectrum
	float tmp = 1-(w-380)/(750-380); // bring w between [0,1]
	return tmp*300; // scale to [0,300] and return
}

glm::vec3 Light::hsv_to_rgb(float h, float s, float v) {
	float chroma = v*s;
	float h_i = h/60.0f;
	float x = chroma*(1.0f - fabs(fmod(h_i,2) - 1.0f));
	float m = v-chroma;
	switch((int)floor(h_i)) {
		case 0: return glm::vec3(chroma+m, x+m, m); break;
		case 1: return glm::vec3(x+m, chroma+m, m); break;
		case 2: return glm::vec3(m, chroma+m, x+m); break;
		case 3: return glm::vec3(m, x+m, chroma+m); break;
		case 4: return glm::vec3(x+m, m, chroma+m); break;
		case 5: return glm::vec3(chroma+m, m, x+m); break;
		default: return glm::vec3(m);
	}
}