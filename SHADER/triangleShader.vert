#version 430


// camera loads its matrices to this block (update each whole frame)
layout(std140) uniform view_projection {
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


attribute vec3 position;
attribute vec4 color;
attribute vec3 normal;

varying vec3 fragPosition; // world space
varying vec4 fragColor;
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

void next() {
	fragMaterialAbsorption = MaterialAbsorption;
	fragMaterialReflection = MaterialReflection;
	fragMaterialTransmission = MaterialTransmission;
	fragMaterialShininess = MaterialShininess;
	fragPosition = world_space(position);
	fragColor = color;
	fragNormal = mat3(model) * normal;
}

void main() {
	gl_Position = clip_space(position);
	next();
}