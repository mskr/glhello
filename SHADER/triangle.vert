#version 430

uniform mat4 ShadowMappingMatrix; //TODO Matrix for each light

// View-procjection matrices
layout(std140) uniform Camera {
	mat4 view;
	mat4 proj;
};

// Model matrix
in mat4 model;

// Material
in vec3 MaterialAbsorption;
in vec3 MaterialReflection;
in float MaterialTransmission;
in float MaterialShininess;
flat varying vec3 fragMaterialAbsorption;
flat varying vec3 fragMaterialReflection;
flat varying float fragMaterialTransmission;
flat varying float fragMaterialShininess;

// Index of light if vertex is part of a light emitter
in int Emitter;
flat varying int fragEmitter;

// Vertex position
attribute vec3 position;
varying vec3 fragPosition; // world space
varying vec4 fragPosLightSpace; //TODO Each light has its own light space

// Vertex normal
attribute vec3 normal;
varying vec3 fragNormal; // world space


// Helper functions

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

void next() {
	fragEmitter = Emitter;
	fragMaterialAbsorption = MaterialAbsorption;
	fragMaterialReflection = MaterialReflection;
	fragMaterialTransmission = MaterialTransmission;
	fragMaterialShininess = MaterialShininess;
	fragPosition = world_space(position);
	fragNormal = mat3(model) * normal;
	fragPosLightSpace = ShadowMappingMatrix * vec4(fragPosition, 1);
}

// Main function
void main() {
	gl_Position = clip_space(position);
	next();
}