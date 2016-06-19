#version 430

/*
* This shader is for point primitives.
* It renders single vertices as little balls.
*/

// camera loads its matrices to this block (update each whole frame)
layout(std140) uniform Camera {
	mat4 view;
	mat4 proj;
};

// world loads model matrix to this vertex attributes (update each instance)
in mat4 model;

in uvec2 Emitter;
flat varying uvec2 fragEmitter;

attribute vec3 position;
varying vec3 fragPosition;

vec4 clip_space(vec3 pos) {
	return proj*view*model*vec4(pos, 1);
}

vec3 world_space(vec3 pos) {
	return vec3(model * vec4(pos, 1));
}

void main() {
	float size = (mat3(model)*vec3(1000)).x; // the particle size
	vec4 clip_space_pos = clip_space(position);
	gl_PointSize = size/clip_space_pos.w; // perspective divide
	gl_Position = clip_space_pos;
	fragPosition = world_space(position);
	fragEmitter = Emitter;
}