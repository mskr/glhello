#version 430

/*
* This shader is for point primitives.
* It renders single vertices as little balls.
*/

uniform int modelID;
uniform uint OcclusionPrePass;

in vec3 fragPosition; // in world space

uniform int num_lights;
layout(std430) buffer light {
	float lights[];
};

flat in int fragEmitter;

vec3 lightXYZ(uint index) {
	return vec3(lights[6*index], lights[6*index+1], lights[6*index+2]);
}

vec3 lightRGB(uint index) {
	return vec3(lights[6*index+3], lights[6*index+4], lights[6*index+5]);
}

bool testEmitter() {
	return fragEmitter >= 0;
}

vec4 emitterColor() {
	return vec4(lightRGB(fragEmitter), 1.0);
}

vec4 lambert(vec3 normal) {
	vec4 sum = vec4(0.0);
	vec3 lightPosition;
	vec3 lightRGB;
	vec3 lightDirection;
	for(int i = 0; i < 6*num_lights; i+=6) {
		lightPosition = vec3(lights[i],lights[i+1],lights[i+2]);
		lightRGB = vec3(lights[i+3],lights[i+4],lights[i+5]);
		lightDirection = lightPosition-fragPosition;
		sum += max(0.1, dot(normalize(normal), normalize(lightDirection))) * lightRGB;
	}
	return sum;
}

vec3 sphere_normal() {
	vec3 normal = vec3(0,0,0);
	normal.xy = gl_PointCoord* 2.0 - vec2(1.0);
	float mag = dot(normal.xy, normal.xy);
	if (mag > 1.0) discard; // kill pixels outside circle
	normal.z = sqrt(1.0-mag);
	return normal;
}

bool testOcclusionPrePass() {
	return OcclusionPrePass != 0;
}

void main() {
	vec3 normal = sphere_normal();
	gl_FragColor = testEmitter() ? emitterColor() : (testOcclusionPrePass() ? vec4(0) : lambert(normal));
}