#version 430

#define M_PI 3.1415926535897932384626433832795

out vec4 outColor;

vec4 defaultColor = vec4(1,0,0,1);

// World image must always be bound to 0
layout(binding = 0) uniform sampler2D world_image;
in vec2 pixel;

// Volumetric Light Scattering Mitchell
layout(binding = 1) uniform sampler2D occlusion_pre_pass_image;
uniform vec2 lightsource;
uniform int NUM_SAMPLES;
uniform float EXPOSURE;
uniform float DENSITY;
uniform float DECAY;

vec4 VolumetricLightScatteringMitchell() {
	vec2 s = pixel - lightsource;
	s *= 1.0 / (NUM_SAMPLES * DENSITY);
	vec4 pixelcolor = texture(occlusion_pre_pass_image, pixel);
	float expDecay = 1.0;
	vec2 smpl = pixel;
	for(int i = 0; i < NUM_SAMPLES; i++) {
		smpl -= s;
		vec4 smplcolor = texture(occlusion_pre_pass_image, smpl);
		smplcolor *= expDecay;
		pixelcolor += smplcolor;
		expDecay *= DECAY;
	}
	return pixelcolor * EXPOSURE + texture(world_image, pixel);
}

// Lightshafts Toth
uniform vec4 CameraPositionLightSpace;
uniform int Toth_N;

vec4 LightShaftsToth(vec4 L) {
	//TODO
	return L;
}

// Sun Shafts Sousa
// Doing the last blur pass and composition here
// Share the lightsource uniform with Mitchell's post process
layout(binding = 2) uniform sampler2D surface_map;
uniform int TAPS;
uniform float stepsize;
uniform float brightness;
vec4 SousaBlur(sampler2D image) {
	vec2 v = lightsource - pixel;
	float l = length(v);
	vec2 stp = stepsize * v / l;
	float MAX_STEPS = l / stepsize;
	float sum = 0.0;
	vec2 p = pixel;
	for(int i = 0; i < TAPS; i ++) {
		if(i >= MAX_STEPS) break;
		sum += texture(image, p).r;
		p += stp;
	}
	return vec4(sum * brightness);
}

vec4 SousaBlend(vec4 sun_shaft_pixel, vec4 world_pixel) {
	return sun_shaft_pixel + world_pixel;
}

void main() {
	if(NUM_SAMPLES != 0) {
		outColor = VolumetricLightScatteringMitchell();
	} else if(Toth_N != 0) {
		outColor = LightShaftsToth(texture(world_image, pixel));
	} else if(TAPS != 0) {
		outColor = SousaBlend(SousaBlur(surface_map), texture(world_image, pixel));
	} else {
		outColor = defaultColor;
	}
}