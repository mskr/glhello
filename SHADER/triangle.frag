#version 430

uniform int modelID;

in vec3 fragPosition;
in vec4 fragColor;
in vec3 fragNormal;

flat in vec3 fragMaterialAbsorption;
flat in vec3 fragMaterialReflection;
flat in float fragMaterialTransmission;
flat in float fragMaterialShininess;

uniform int num_lights;
layout(std430) buffer light {
	float lights[];
};

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

vec4 spectral_response() {
	return vec4(fragMaterialReflection - fragMaterialAbsorption, 1.0 - fragMaterialTransmission);
}

void main() {
	if(modelID == 0) gl_FragColor = fragColor * lambert();
	else             gl_FragColor = spectral_response() * lambert();
}