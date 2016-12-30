#version 430

#define M_PI 3.1415926535897932384626433832795

layout(binding = 0) uniform sampler2D ShadowMap;

uniform vec2 viewport;
uniform vec3 cameraPos; // in world space
uniform int modelID;

flat in vec4 fragCameraPosLightSpace;

//TODO ShadowMapping: Either pass array from vertex shader or do matrix multiplication on fragment shader
in vec4 fragPosLightSpace;
uniform mat4 ShadowMappingMatrix;

in vec4 fragPosViewSpace;

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

// float LinearizeDepth(float depth) {
// 	float z = depth * 2.0 - 1.0; // Back to NDC 
// 	float tmp = (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
// 	return tmp / far_plane;
// }

bool testShadow(vec4 positionLightSpace) {
	vec3 ndc = positionLightSpace.xyz / positionLightSpace.w; // perspective divide
	// Fragment can be outside light's view frustum...
	if(ndc.x < -1 || ndc.x > 1 || ndc.y < -1 || ndc.y > 1 || ndc.z > 1 || ndc.z < 0)
		return  false; // ... then it is considered NOT in shadow
	ndc = ndc * 0.5 + 0.5; // NDCs to texture coordinates [0,1]^2

	vec2 texelSize = 1.0 / textureSize(ShadowMap, 0);

	float closestDepth = texture(ShadowMap, ndc.xy).r;

	// float shadow = 0.0;
	// for(int i = -1; i <= 1; i += 1) {
	// 	for(int j = -1; j <= 1; j+= 1) {
	// 		float closestDepth = texture(ShadowMap, ndc.xy + vec2(i,j) * texelSize).r;
	// 		shadow += ((ndc.z - 0.0001) > closestDepth) ? 0.0 : 1.0;
	// 	}
	// }
	// return shadow/9.0;

	float currentDepth = ndc.z;
	return closestDepth < (currentDepth-0.0001);
}

// scattering phase function (returns probability for scattering from x in viewDir)
// float P(vec4 x, vec4 viewDir) {
// 	// Approx rayleigh by cos^2
// 	float cosinus = dot(normalize(x), normalize(viewDir));
// 	cosinus *= cosinus;
// 	return cosinus;
// }

// vec4 LightShaftsToth(vec4 L0, int lightIndex) {
// 	int N = 100;
// 	float density = 0.01;
// 	float albedo = .8;
// 	vec3 beta = vec3(.0695, .118, .244); // scattering coefficient for air
// 	float s = length(fragCameraPosLightSpace - fragPosLightSpace); // Distance camera-surface
// 	float dl = s / float(N); // Step size
// 	vec4 L = L0; // Output radiance
// 	L *= exp(-s * density); // Total extinction along s
// 	// Init light space vectors
// 	vec4 viewDirLightSpace = fragCameraPosLightSpace - fragPosLightSpace;
// 	viewDirLightSpace = normalize(viewDirLightSpace);
// 	vec4 x = fragPosLightSpace;
// 	// Start ray marching
// 	for(float l = s - dl; l >= 0; l -= dl) {
// 		x += viewDirLightSpace * dl;
// 		float v = testShadow(x) ? 0.1 : 1.0;
// 		float d = length(x); // Distance light-particle
// 		float Lin = exp(-d * density) * v * 9000/4/M_PI/d/d;
// 		float Li = Lin * density * albedo * P(x, viewDirLightSpace);
// 		// l == Distance particle-camera
// 		L += Li /** vec4(beta*3,1)*/ * exp(-l * density) * dl;
// 		//TODO implement as post processing with deferred shading-like approach
// 	}
// 	return L;
// }

//TODO Heney Grenstein mit verschiedenen g und Rayleigh screenshotten
// Anisotropie herausstellen
uniform float HG_g;
float scatteringPhaseFunction(vec3 particlePos, vec3 viewDir) {
	float cosine = dot(normalize(particlePos), normalize(viewDir));
	// return pow(cosine,2);
	float term1 = pow((1.0 - HG_g), 2);
	float term2 = 1.0 + pow(HG_g, 2) - 2 * HG_g * cosine;
	float term3 = 4 * M_PI * pow(term2, 1.5);
	return term1/term2;
}

uniform int Toth_n;
uniform float Toth_P;
uniform float Toth_albedo;
uniform float Toth_density;
uniform float Toth_scaling;

vec4 ParticipatingMediaToth(vec4 L) {
	float P = Toth_P;
	int N = Toth_n;
	float density = Toth_density;
	float albedo = Toth_albedo;
	vec3 beta = vec3(.0695, .118, .244); // scattering coefficient for air
	vec4 viewDirLightSpace = fragCameraPosLightSpace - fragPosLightSpace;
	float s = length(viewDirLightSpace);
	viewDirLightSpace = normalize(viewDirLightSpace);
	float ds = s / float(N);
	// Calculate extinction of the surface radiance
	L *= exp(-s * density);
	vec4 x = fragPosLightSpace;
	while(s >= 0) {
		s -= ds;
		x += viewDirLightSpace * ds;
		float S = testShadow(x) ? 0.0 : 1.0;
		float d = length(x);
		L += S * /*vec4(beta*4,1) **/ 
			albedo * 
			density * 
			// Calculate extinction of inscattered radiance to the point on the ray
			exp(-d * density) * 
			// Calculate extincion of inscattered radiance to the eye
			exp(-s * density) * 
			P/4/M_PI/pow(d,2) * 
			scatteringPhaseFunction(x.xyz, viewDirLightSpace.xyz);
	}
	return Toth_scaling * L;
}

void main() {
	if(modelID == 7) {
		// skybox, skip shading
		gl_FragColor = (vec4(.0695, .118, .244, 1)*4); //sky color used for screenshot
		// gl_FragColor = (vec4(0));
	} else if(testEmitter()) {
		vec4 emittedLight = emitterColor();
		gl_FragColor = (emittedLight);
	} else if(testShadow(fragPosLightSpace)) {
		vec4 ambientLight = specular_response() * 0.2;
		gl_FragColor = vec4(0,0,0,1);//(ambientLight);
	} else {
		vec4 reflectedLight = specular_response() * lambert();
		gl_FragColor = (reflectedLight);// * testShadow(fragPosLightSpace);
	}
	if(Toth_n > 0) gl_FragColor = ParticipatingMediaToth(gl_FragColor);
}