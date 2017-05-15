uniform int num_lights;

layout(std430) buffer light {
	float lights[]; // XYZ and RGB in alternating order
};

flat in int fragEmitter;

/*
vec3 lightXYZ(uint index) {
	return vec3(lights[6*index], lights[6*index+1], lights[6*index+2]);
}

vec3 lightRGB(uint index) {
	return vec3(lights[6*index+3], lights[6*index+4], lights[6*index+5]);
}
*/

bool testEmitter() {
	return fragEmitter >= 0;
}

vec4 emitterColor() {
	return vec4(lightRGB(fragEmitter), 1.0);
}

vec4 sum = vec4(0.0);
vec3 lightPosition;
vec3 lightRGB;
vec3 lightDirection;

vec4 calcLight(int i) {
	lightPosition = vec3(lights[i],lights[i+1],lights[i+2]);
	lightRGB = vec3(lights[i+3],lights[i+4],lights[i+5]);
	lightDirection = lightPosition-fragPosition;
	return max(0.0, dot(normalize(fragNormal), normalize(lightDirection))) * lightRGB;
}

vec4 Light() {
	for(int i = 0; i < 6*num_lights; i+=6) {
		vec4 lightIntensity = calcLight(i, fragPosition);
		//TODO How can modules like ShadowMapping alter lightIntensity here!?
		sum += lightIntensity;
	}
	return sum;
}