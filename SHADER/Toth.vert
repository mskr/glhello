#version 430

in vec3 position;
in mat4 model;

layout(std140) uniform Camera {
	mat4 view;
	mat4 proj;
};

layout(location = 0) uniform mat4 lightSpaceMatrix;
layout(location = 1) uniform int pass;

out vec4 positionLightSpace;

void main() {
	if(pass == 1) {
		// Create shadow map
		gl_Position = lightSpaceMatrix * model * vec4(position,1);
	} else if(pass == 2) {
		// Create surface map
		gl_Position = proj * view * model * vec4(position,1);
		positionLightSpace = lightSpaceMatrix * model * vec4(position,1);
	}
}