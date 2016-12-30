#version 430

layout(location = 1) uniform int pass;

in vec4 positionLightSpace;

void main() {
	if(pass == 1) {
		// Write depth in shadow map
	} else if(pass == 2) {
		// Write light space position in surface map
		gl_FragColor = positionLightSpace;
	}
}