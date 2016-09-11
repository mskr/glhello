#version 430

in vec2 pixel;
uniform sampler2D depthMap;

// uniform float near;
// uniform float far;

// float LinearizeDepth(float depth)
// {
// 	float z = depth * 2.0 - 1.0; // Back to NDC 
// 	return (2.0 * near * far) / (far + near - z * (far - near));	
// }

void main() {
	// float depthValue = texture(depthMap, pixel).r;
	// gl_FragColor = vec4(vec3(LinearizeDepth(depthValue) / far), 1.0); // perspective
	// gl_FragColor = vec4(vec3(depthValue), 1.0);
	gl_FragColor = texture(depthMap, pixel);
}