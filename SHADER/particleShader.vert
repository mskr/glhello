#version 430

/*
* This shader is for point primitives.
* It renders single vertices as little balls.
*/

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

varying vec3 fragPosition; // world space
varying vec4 fragColor;

vec4 clip_space(vec3 pos) {
	return proj*view*model*vec4(pos, 1);
}

vec3 world_space(vec3 pos) {
	return vec3(model * vec4(pos, 1));
}

void main() {
	float size = (mat3(model)*vec3(500)).x; // the particle size
	fragPosition = world_space(position);
	fragColor = color;
	vec4 clip_space_pos = clip_space(position);
	gl_PointSize = size/clip_space_pos.w; // perspective divide
	gl_Position = clip_space_pos;
}