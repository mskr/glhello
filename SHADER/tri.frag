#version 430

in vec3 fragPosition; // in world space
in vec3 fragNormal; // in world space

flat in vec3 fragMaterialAbsorption;
flat in vec3 fragMaterialReflection;
flat in float fragMaterialTransmission;
flat in float fragMaterialShininess;

flat in int fragEmitter;

uniform int num_lights;
layout(std430) buffer light {
	float lights[]; // XYZ and RGB in alternating order
};

vec3 lightXYZ(uint index) {
	return vec3(lights[6*index], lights[6*index+1], lights[6*index+2]);
}

vec3 lightRGB(uint index) {
	return vec3(lights[6*index+3], lights[6*index+4], lights[6*index+5]);
}

float lightPower(uint index) {
	vec3 rgb = lightRGB(index);
	return (rgb.r + rgb.g + rgb.b) / 3.0;
}

bool testEmitter() {
	return fragEmitter >= 0;
}

vec4 emitterColor() {
	return vec4(lightRGB(fragEmitter), 1.0);
}

vec4 lambert() {
	vec4 sum = vec4(0.0);
	vec3 lightPosition;
	vec3 lightRGB;
	vec3 lightDirection;
	for(int i = 0; i < 6*num_lights; i+=6) {
		lightPosition = vec3(lights[i],lights[i+1],lights[i+2]);
		lightRGB = vec3(lights[i+3],lights[i+4],lights[i+5]);
		lightDirection = lightPosition-fragPosition;
		sum += max(0.0, dot(normalize(fragNormal), normalize(lightDirection))) * lightRGB;
	}
	return sum;
}

vec4 phong() {
	return vec4(0); // not implemented
}

vec4 specular_response() {
	return vec4(fragMaterialReflection - fragMaterialAbsorption, 1.0 - fragMaterialTransmission);
}

void main() {
	if(testEmitter()) {
		gl_FragColor = emitterColor();
	} else {
		gl_FragColor = specular_response() * lambert();
	}
}