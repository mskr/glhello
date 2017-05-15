#version 430

// FRAGMENT SHADER FOR TRIANGLE MODELTYPE
//TODO When modules define modeltype, this file is not needed,
// because it can be compiled out of all active modules

in vec3 fragPosition; // in world space
in vec3 fragNormal; // in world space



//TODO Material as a module
flat in vec3 fragMaterialAbsorption;
flat in vec3 fragMaterialReflection;
flat in float fragMaterialTransmission;
flat in float fragMaterialShininess;

vec4 specular_response() {
	return vec4(fragMaterialReflection - fragMaterialAbsorption, 1.0 - fragMaterialTransmission);
}

void main() {
	//TODO All modules supporting this modeltype should be called here
	//     and results are added
	// PROBLEM: Many modules depend on light loop!
	if(testEmitter()) {
		gl_FragColor = emitterColor();
	} else {
		gl_FragColor = specular_response() * lambert();
	}
}