#version 430

uniform uint OcclusionPrePass;

// camera loads its matrices to this block (update each whole frame)
layout(std140) uniform Camera {
	mat4 view;
	mat4 proj;
};

// world loads model matrix to this vertex attributes (update each instance)
in mat4 model;


in vec3 MaterialAbsorption;
in vec3 MaterialReflection;
in float MaterialTransmission;
in float MaterialShininess;
flat varying vec3 fragMaterialAbsorption;
flat varying vec3 fragMaterialReflection;
flat varying float fragMaterialTransmission;
flat varying float fragMaterialShininess;


in uvec2 Emitter;
flat varying uvec2 fragEmitter;

attribute vec3 position;
attribute vec3 normal;

varying vec3 fragPosition; // world space
varying vec3 fragNormal; // world space


vec4 clip_space(vec3 pos) {
	return proj*view*model*vec4(pos, 1);
}

vec3 world_space(vec3 pos) {
	return vec3(model * vec4(pos, 1));
}

mat3 normal_matrix() {
	//TODO better do the inverse operation on the cpu and not for every vertex
	return mat3(transpose(inverse(model)));
}

bool testOcclusionPrePass() {
	return OcclusionPrePass != 0;
}

void next() {
	fragEmitter = Emitter;
	if(testOcclusionPrePass()) return;
	fragMaterialAbsorption = MaterialAbsorption;
	fragMaterialReflection = MaterialReflection;
	fragMaterialTransmission = MaterialTransmission;
	fragMaterialShininess = MaterialShininess;
	fragPosition = world_space(position);
	fragNormal = mat3(model) * normal;
}

void main() {
	gl_Position = clip_space(position);
	next();
}