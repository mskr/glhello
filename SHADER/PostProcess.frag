#version 430

layout(binding=0) uniform sampler2D world_image;
layout(binding=1) uniform sampler2D occlusion_pre_pass_image;

in vec2 pixel;
uniform vec2 lightsource;

uniform int NUM_SAMPLES;
uniform float EXPOSURE;
uniform float DENSITY;
uniform float WEIGHT;
uniform float DECAY; //TODO behaves weird if != 1

void main() {
	vec2 s = (pixel - lightsource);
	s *= 1.0 / NUM_SAMPLES * DENSITY;
	vec4 pixelcolor = texture(occlusion_pre_pass_image, pixel);
	float expDecay = 1.0;
	vec2 smpl = pixel;
	for(int i = 0; i < NUM_SAMPLES; i++) {
		smpl -= s;
		vec4 smplcolor = texture(occlusion_pre_pass_image, smpl);
		smplcolor *= expDecay * WEIGHT;
		pixelcolor += smplcolor;
		expDecay *= DECAY;
	}
	gl_FragColor = pixelcolor * EXPOSURE + texture(world_image, pixel);
}