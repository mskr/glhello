#version 430

// camera loads its matrices to this block (update each whole frame)
layout(std140) uniform view_projection {
	mat4 view;
	mat4 proj;
};

// world loads model matrix to this vertex attributes (update each instance)
in vec4 model_matrix_column_1;
in vec4 model_matrix_column_2;
in vec4 model_matrix_column_3;
in vec4 model_matrix_column_4;

mat4 model = mat4(
	model_matrix_column_1, 
	model_matrix_column_2, 
	model_matrix_column_3, 
	model_matrix_column_4);

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
	return mat3(transpose(inverse(model))); //TODO better do the inverse operation on the cpu and not for every vertex
}

void main() {
	fragPosition = world_space(position);
	fragColor = color;
	fragNormal = mat3(model) * normal;
	gl_Position = clip_space(position);
}